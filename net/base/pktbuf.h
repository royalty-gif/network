/**
 *  @brief: 数据包管理
 */

#ifndef _PKTBUF_H_
#define _PKTBUF_H_

#include <stdint.h>
#include "net_cfg.h"
#include "nqueue.h"
#include "net_err.h"

/**
 *  @brief: 数据块类型
 */

typedef struct _pktblk_t {
    nlist_t node;    // 链表节点
    int size;        // 数据块大小
    uint8_t* pdata;  // 数据块读写指针
    uint8_t payload[PKTBUF_BLK_SIZE];  // 数据缓冲区
} pktblk_t;


/**
 *  @brief: 网络包类型
 */

typedef struct _pktbuf_t {
    int total_size;       // 数据包总大小
    nqueue_t blk_queue;   // 数据块队列
    nlist_t node;         // pktbuf链表节点

    int ref;              // 引用计数
    int pos;              // 当前位置总的偏移量
    pktblk_t* curr_blk;   // 当前数据块
    uint8_t* blk_offset;  // 当前数据块偏移量
} pktbuf_t;

/**
 *  @brief: 获取当前block的下一个子包
 */
static pktblk_t* pktblk_next(pktblk_t* blk) {
    nlist_t* next = blk->node.next;
    return nlist_entry(next, pktblk_t, node);
}

/**
 *  @brief: 获取pktbuf的第一个block
 */
static pktblk_t* pktbuf_first_blk(pktbuf_t* pbuf) {
    nlist_t* first = nqueue_front(&pbuf->blk_queue);
    return nlist_entry(first, pktblk_t, node);
}

/**
 *  @brief: 获取buf第一个数据块
 */
static uint8_t* pktbuf_data(pktbuf_t* buf) {
    pktblk_t* first = pktbuf_first_blk(buf);
    return first ? first->pdata : (uint8_t*)0;
}

net_err_t pktbuf_init(void);
pktbuf_t* pktbuf_alloc(int size);
void pktbuf_free(pktbuf_t * buf);
void pktbuf_reset_acc(pktbuf_t* pbuf);

net_err_t pktbuf_add_header(pktbuf_t* pbuf, int size, int cont);
net_err_t pktbug_remove_header(pktbuf_t* pbuf, int size);

int pktbuf_write(pktbuf_t* buf, uint8_t* src, int size);
int pktbuf_read(pktbuf_t* buf, uint8_t* dest, int size);
net_err_t pktbuf_seek(pktbuf_t *buf, int offset);

#endif