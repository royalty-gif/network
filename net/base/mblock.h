#ifndef _MBLOCK_
#define _MBLOCK_

#include "nqueue.h"
#include "nlocker.h"
#include "nsem.h"

typedef struct _mblock_t {
    void* start;
    nqueue_t free_queue;
    nlocker_t locker;
    sys_sem_t alloc_sem;
} mblock_t;


net_err_t mblock_init (mblock_t* mblock, void* mem, int blk_size, int cnt, nlocker_type_t share_type);
void* mblock_alloc(mblock_t* mblock, int ms);
int mblock_free_blk_cnt(mblock_t* mblock);
void mblock_free(mblock_t* mblock, void *block);
void mblock_destroy(mblock_t* mblock);

#endif