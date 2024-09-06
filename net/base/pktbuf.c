#include "pktbuf.h"
#include "net_cfg.h"
#include "net_err.h"
#include "nlist.h"
#include "nlocker.h"
#include "mblock.h"
#include "log.h"
#include "nqueue.h"
#include "sys_plat.h"
#include "utility.h"
#include <stdint.h>

static nlocker_t locker;    // 分配与回收的锁
static mblock_t block_list; // 空闲数据块列表
static pktblk_t block_buffer[PKTBUF_BLK_CNT];
static mblock_t pktbuf_list; // 空闲网络包列表
static pktbuf_t pktbuf_buffer[PKTBUF_BUF_CNT];

// 数据包位置枚举
typedef enum {
    PACKET_INSERT_FRONT = 0,
    PACKET_INSERT_REAR,
} pkt_pos_t;

net_err_t pktbuf_init(void) {
    info("init packet buf init...");

    net_err_t ret = nlocker_init(&locker, NLOCKER_THREAD);
    if( ret != NET_ERR_OK ) {
        error("nlocker init failed!");
        goto nlocker_failed;
    }

    ret = mblock_init(&block_list, block_buffer, sizeof(pktblk_t), PKTBUF_BLK_CNT, NLOCKER_NONE);
    if( ret != NET_ERR_OK ) {
        error("packet block list init failed!");
        goto block_list_failed;
    }

    ret = mblock_init(&pktbuf_list, pktbuf_buffer, sizeof(pktbuf_t), PKTBUF_BUF_CNT, NLOCKER_NONE);
    if( ret != NET_ERR_OK ) {
        error("packet buf list init failed!");
        goto pktbuf_list_failed;
    }

    info("init done!");
    return ret;

pktbuf_list_failed:
    mblock_destroy(&pktbuf_list);
block_list_failed:
    nlocker_destroy(&locker);
nlocker_failed:
    return ret;
}

/**
 *  @brief: 获取当前block的下一个子包
 */
static pktblk_t* pktblk_next(pktblk_t* blk) {
    nlist_t* next = blk->node.next;
    return container_of(next, pktblk_t, node);
}

/**
 *  @brief: 获取pktbuf的第一个block
 */
static pktblk_t* pktbuf_first_blk(pktbuf_t* pbuf) {
    nlist_t* next = nqueue_front(&pbuf->blk_queue);
    return container_of(next, pktblk_t, node);
}

/**
 *  @brief: 回收block链表
 */
static void pktblk_free_list(pktblk_t* blk_list) {
    while( blk_list ) {
        pktblk_t* next = pktblk_next(blk_list);
        mblock_free(&block_list, blk_list);
        blk_list = next;
    }
}

/**
 *  @brief: 释放一个block
 */
static void pktblk_free(pktblk_t* blk) {
    nlocker_lock(&locker);
    mblock_free(&block_list, blk);
    nlocker_unlock(&locker);
}

/**
 *  @brief: 分配一个空闲的block
 */
static pktblk_t* pktblk_alloc(void) {
    nlocker_lock(&locker);
    pktblk_t* block = (pktblk_t*)mblock_alloc(&block_list, -1);
    nlocker_unlock(&locker);

    if( block ) {
        block->size = 0;
        block->pdata = (uint8_t*)0;
        nlist_init(&block->node);
    } else {
        error("no memory to alloc packet blk...");
    }

    return block;
}

/**
 *  @brief: 分配一个数据包缓冲链
 */
static pktblk_t* pktblk_alloc_list(int size, pkt_pos_t pos) {
    pktblk_t* first_block = (pktblk_t*)0;
    pktblk_t* pre_block = (pktblk_t*)0;

    while( size ) {
        pktblk_t* new_block = pktblk_alloc();
        if( !new_block ) {
            error("no memory to alloc packet blk, size: %d", size);

            // 释放整条数据块链表
            if( first_block ) {
                nlocker_lock(&locker);
                pktblk_free_list(first_block);
                nlocker_unlock(&locker);
            }

            return (pktblk_t*)0;
        }

        int curr_size = size > PKTBUF_BLK_SIZE ? PKTBUF_BLK_SIZE : size;
        new_block->size = curr_size;

        if( pos == PACKET_INSERT_FRONT ) {
            // 将数据包插到链表头部
            new_block->pdata = new_block->payload + PKTBUF_BLK_SIZE - curr_size;

            if( first_block ) {
                nlist_insert_before(&first_block->node, &new_block->node);
            }

            first_block = new_block;
        } else if( pos == PACKET_INSERT_REAR ) {
            // 将数据包插到链表尾部
            if( !first_block ) {
                first_block = new_block;
            }

            // new_block->pdata = new_block->payload;
            new_block->pdata = new_block->payload + PKTBUF_BLK_SIZE - curr_size;
            if( pre_block ) {
                nlist_insert_after(&pre_block->node, &new_block->node);
            }

            pre_block = new_block;
        }
        
        size -= curr_size;
    }

    return first_block;
}

/**
 *  @brief: 将block链表插入到buf中
 */

static void pktbuf_insert_blk_list(pktbuf_t* pbuf, pktblk_t* blk_list, pkt_pos_t pos) {

    pktblk_t* curr_blk = blk_list;
    do {
        // 不断从blk_list中取出块，然后插入到buf的后边
        // 后续可能要进行改进，直接将整条链表插入即可
        pktblk_t* next = pktblk_next(curr_blk);
        
        if( pos == PACKET_INSERT_FRONT ) {
            nqueue_push_front(&pbuf->blk_queue, &curr_blk->node);
        } else if( pos == PACKET_INSERT_REAR ) {
            nqueue_push_back(&pbuf->blk_queue, &curr_blk->node);
        }
        pbuf->total_size += curr_blk->size;

        curr_blk = next;
    } while( curr_blk != blk_list );
}

/**
 *  @brief: 准备pktbuf的读写
 */
void pktbuf_reset_acc(pktbuf_t* pbuf) {
    if ( pbuf ) {
        pbuf->pos = 0;
        pbuf->curr_blk = pktbuf_first_blk(pbuf);
        pbuf->blk_offset = pbuf->curr_blk ? pbuf->curr_blk->pdata : (uint8_t *)0;
    }
}

/**
 *  @brief: 显示和检查buf的情况
 */

static void display_check_buf(pktbuf_t* pbuf) {
    if( !pbuf ) {
        error("buf is invaild...");
        return;
    }

    info("packet buf total size: %d", pbuf->total_size);
    info("queue length: %d", nquene_length(&pbuf->blk_queue));

    pktblk_t* first_blk = pktbuf_first_blk(pbuf);
    pktblk_t* curr = first_blk;
    nlist_node_t* head = &pbuf->blk_queue.head;

    int total_size = 0;
    while( &curr->node != head ) {
        total_size += curr->size;
        curr = pktblk_next(curr);
    }

    info("total_size: %d", total_size);
} 

/**
 *  @brief: 申请pktbuf
 */
pktbuf_t* pktbuf_alloc(int size) {
    // 分配结构
    nlocker_lock(&locker);
    pktbuf_t* pbuf = (pktbuf_t*)mblock_alloc(&pktbuf_list, -1);
    nlocker_unlock(&locker);
    if( !pbuf) {
        error("no memory to alloc packet buf...");
        return (pktbuf_t*)0;
    }

    // 字段初始化
    pbuf->ref = 1;
    pbuf->total_size = 0;
    nqueue_init(&pbuf->blk_queue);
    nlist_init(&pbuf->node);

    // 申请空间
    if( size ) {
        pktblk_t* block = pktblk_alloc_list(size, PACKET_INSERT_REAR);
        if( !block ) {
            mblock_free(&pktbuf_list, pbuf);
            return (pktbuf_t*)0;
        }

        pktbuf_insert_blk_list(pbuf, block, PACKET_INSERT_REAR);
    }

    // 设置读写情况
    pktbuf_reset_acc(pbuf);

    // 检查buf
    display_check_buf(pbuf);

    return pbuf;
}

/**
 *  @brief: 释放pktbuf
 */
void pktbuf_free (pktbuf_t* pbuf) {
    nlocker_lock(&locker);

    if( --pbuf->ref == 0 ) {
        pktblk_free_list(pktbuf_first_blk(pbuf));
        mblock_free(&pktbuf_list, pbuf);
    }

    nlocker_unlock(&locker);
}

/**
 *  @brief: 为数据包新增一个数据头
 */
net_err_t pktbuf_add_header(pktbuf_t* pbuf, int size, int cont) {
    if( !pbuf && !pbuf->ref ) {
        error("buf is invaild...");
        return NET_ERR_PARAM;
    }

    pktblk_t* block = pktbuf_first_blk(pbuf);

    // 检查头部是否足够
    int remain_size = block->pdata - block->payload;
    info("remain size: %d", remain_size);
    if( size <= remain_size ) {
        block->size += size;
        block->pdata -= size;
        pbuf->total_size += size;

        display_check_buf(pbuf);
        return NET_ERR_OK;
    }

    // 没有足够的空间，需要额外分配块添加到头部
    if( cont ) {
        // 要求连续时，但是大小超过一个块的大小，分配失败
        if( size > PKTBUF_BLK_SIZE ) {
            error("contious and header size is too big, size: %d", size);
            return NET_ERR_NONE;
        }

        // 新分配一个块，原本的区空间不影响
        block = pktblk_alloc_list(size, PACKET_INSERT_FRONT);
        if( !block ) {
            error("no memory to alloc packet blk, size: %d", size);
            return NET_ERR_NONE;
        }

    } else {
        // 非连续分配，则将前边的空间利用起来，再分配剩下的大小
        block->pdata = block->payload;
        block->size += remain_size;
        pbuf->total_size += remain_size;
        size -= remain_size;

        // 再分配未用的空间
        block = pktblk_alloc_list(size, PACKET_INSERT_FRONT);
        if( !block ) {
            error("no memory to alloc packet blk, size: %d", size);
            return NET_ERR_NONE;
        }
    }

    // 将数据块加入列表的头部
    pktbuf_insert_blk_list(pbuf, block, PACKET_INSERT_FRONT);

    display_check_buf(pbuf);

    return NET_ERR_OK;
}

/**
 *  @brief: 移除数据包头
 */
net_err_t pktbug_remove_header(pktbuf_t* pbuf, int size) {
    if( !pbuf && !pbuf->ref ) {
        error("buf is invaild...");
        return NET_ERR_PARAM;
    }

    pktblk_t* block = pktbuf_first_blk(pbuf);
    while( size ) {
        // 判断当前的包是否足够减去头，满足则直接在当前表操作即可
        if( size < block->size ) {
            block->pdata += size;
            block->size -= size;
            pbuf->total_size -= size;
            break;
        }

        // 不满足，则将当数据包释放了
        pktblk_t* next_blk = pktblk_next(block);
        int curr_size = block->size;

        nqueue_pop_front(&pbuf->blk_queue);
        pktblk_free(block);

        size -= curr_size;
        pbuf->total_size -= curr_size;
        block = next_blk;
    }

    display_check_buf(pbuf);

    return NET_ERR_OK;
}


// 获取pktbuf的有效数据空间大小
static int total_blk_remain(pktbuf_t* buf) {
    return buf->total_size - buf->pos;
}

// 获取当前的block余下的空间大小
static int curr_blk_remain(pktbuf_t* buf) {
    pktblk_t* block = buf->curr_blk;
    if (!block) {
        return 0;
    }

    return (int)(buf->curr_blk->pdata + block->size - buf->blk_offset);
}

// 移动指针位置，如果是跨越一个包，则仅移动到下一个包的开头
static void move_forward(pktbuf_t* buf, int size) {
    pktblk_t* curr_blk = buf->curr_blk;

    // 调整读写位置
    buf->pos += size;
    buf->blk_offset += size;

    // 可能超过当前块，所以要移动到下一块
    if(buf->blk_offset >= curr_blk->pdata + curr_blk->size) {
        buf->curr_blk = pktblk_next(curr_blk);
        if( buf->curr_blk ) {
            buf->blk_offset = buf->curr_blk->pdata;
        } else {
            // 下一块为空
            buf->blk_offset = (uint8_t*)0;
        }
    }
}

/**
 *  @brief: 将src的数据写入到buf中
 */
int pktbuf_write(pktbuf_t *buf, uint8_t *src, int size) {
    // 参数检查
    if(!src || !size) {
        return NET_ERR_PARAM;
    }

    int remain_size = total_blk_remain(buf);
    if( remain_size < size ) {
        error("size error, remain_size(%d) < size(%d)", remain_size, size);
        return NET_ERR_SIZE;
    }

    // 循环写入数据
    while( size > 0 ) {
        int blk_size = curr_blk_remain(buf);

        int curr_copy_size = size > blk_size ? blk_size : size;
        plat_memcpy(buf->blk_offset, src, curr_copy_size);

        // 移动指针
        move_forward(buf, curr_copy_size);
        src += curr_copy_size;
        size -= curr_copy_size;
    }

    return NET_ERR_OK;
}


/**
 *  @brief: 从当前位置读出指定数据量
 */
int pktbuf_read(pktbuf_t *buf, uint8_t *dest, int size) {
    // 参数检查
    if(!dest || !size) {
        return NET_ERR_OK;
    }

    int remain_size = total_blk_remain(buf);
    if( remain_size < size ) {
        error("size error, remain_size(%d) < size(%d)", remain_size, size);
        return NET_ERR_SIZE;
    }

    // 循环读取所有量
    while (size > 0) {
        int blk_size = curr_blk_remain(buf);
        int curr_copy = size > blk_size ? blk_size : size;
        plat_memcpy(dest, buf->blk_offset, curr_copy);

        // 超出当前buf，移至下一个buf
        move_forward(buf, curr_copy);

        dest += curr_copy;
        size -= curr_copy;
    }

    return NET_ERR_OK;
}

/**
 *  @brief: 移动当前的读写位置到offset偏移处
 */
net_err_t pktbuf_seek(pktbuf_t *buf, int offset) {

}