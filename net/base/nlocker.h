#ifndef _NLOCKER_H_
#define _NLOCKER_H_

#include <pthread.h>
#include "net_err.h"

typedef pthread_mutex_t* sys_mutex_t;

typedef enum {
    NLOCKER_NONE,
    NLOCKER_THREAD,   // 线程之间共享的锁
    NLOCKER_INT,      // 中断相关的锁
} nlocker_type_t;

typedef struct _nlocker_t {
    nlocker_type_t type;
    sys_mutex_t mutex;
} nlocker_t;

net_err_t nlocker_init(nlocker_t* locker, nlocker_type_t type);
void nlocker_destroy(nlocker_t * locker);
void nlocker_lock(nlocker_t * locker);
void nlocker_unlock(nlocker_t * locker);

#endif