#include "mblock.h"
#include <stdint.h>
#include "log.h"
#include "net_err.h"
#include "nlocker.h"
#include "nqueue.h"
#include "nsem.h"

net_err_t mblock_init (mblock_t* mblock, void* mem, int blk_size, int cnt, nlocker_type_t share_type) {

    net_err_t ret = NET_ERR_OK;
    
    // 使用到了链表，因此块大小不能小于链表节点的大小
    if( blk_size < sizeof(nlist_node_t) ) {
        error("init size error!");
        ret = NET_ERR_SIZE;
        goto size_error;
    }

    // 初始化锁
    ret = nlocker_init(&mblock->locker, share_type);
    if( ret != NET_ERR_OK ) {
        error("locker init error!");
        goto locker_failed;
    }

    // 初始化消息队列
    uint8_t* buf = (uint8_t*)mem;
    nqueue_init(&mblock->free_queue);
    for(int i = 0; i < cnt; i++, buf += blk_size) {
        nqueue_node_t* node = (nqueue_node_t*)buf;
        nqueue_node_init(node);
        nqueue_push_back(&mblock->free_queue, node);
    }

    // 初始化信号量
    if( share_type != NLOCKER_NONE ) {
        mblock->alloc_sem = sys_sem_create(cnt);
        if (mblock->alloc_sem == SYS_SEM_INVALID ) {
            error("sem init error!");
            ret = NET_ERR_SYS;
            goto sem_failed;
        }
    }

    return ret;

sem_failed:
    nlocker_destroy(&mblock->locker);
locker_failed:
size_error:
    return ret;
}

void* mblock_alloc(mblock_t* mblock, int ms) {
    if( (ms < 0) || (mblock->locker.type == NLOCKER_NONE) ) {
        if( !mblock_free_blk_cnt(mblock) ) {
            info("free block count is zero!");
            return NULL;
        }
    }

    if( mblock->locker.type != NLOCKER_NONE ) {
        sys_sem_wait(mblock->alloc_sem, ms);
    }

    // 获取分配项
    nlocker_lock(&mblock->locker);
    nqueue_node_t* node = nqueue_pop_front(&mblock->free_queue);
    nlocker_unlock(&mblock->locker);

    return node;
}

int mblock_free_blk_cnt(mblock_t* mblock) {
    nlocker_lock(&mblock->locker);
    int count = nquene_length(&mblock->free_queue);
    nlocker_unlock(&mblock->locker);
    return count;
}

void mblock_free(mblock_t* mblock, void* block) {
    nlocker_lock(&mblock->locker);
    nqueue_push_back(&mblock->free_queue, block);
    nlocker_unlock(&mblock->locker);

    // 释放掉一个资源，通知其它任务该资源可用
    if (mblock->locker.type != NLOCKER_NONE) {
        sys_sem_notify(mblock->alloc_sem);
    }
}

void mblock_destroy(mblock_t* mblock) {
    if ( mblock->locker.type != NLOCKER_NONE ) {
        sys_sem_free(mblock->alloc_sem);
        nlocker_destroy(&mblock->locker);
    }
}