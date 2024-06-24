#include "nlocker.h"
#include <malloc.h>
#include <pthread.h>
static sys_mutex_t sys_mutex_create(void) {
    sys_mutex_t mutex = (sys_mutex_t)malloc(sizeof(pthread_mutex_t));

    int err = pthread_mutex_init(mutex, NULL);
    if( err < 0 ) {
        return SYS_MUTEX_INVALID;
    }

    return mutex;
}

static void sys_mutex_destroy(sys_mutex_t mutex) {
    pthread_mutex_destroy(mutex);
    free(mutex);
}

net_err_t nlocker_init(nlocker_t* locker, nlocker_type_t type) {
    if( type == NLOCKER_THREAD ) {
        sys_mutex_t mutex = sys_mutex_create();
        if( mutex == SYS_MUTEX_INVALID ) {
            return NET_ERR_SYS;
        }

        locker->mutex = mutex;
    }

    locker->type = type;

    return NET_ERR_OK;
}

void nlocker_destroy(nlocker_t* locker) {
    if( locker->type == NLOCKER_THREAD ) {
        sys_mutex_destroy(locker->mutex);
    }
}

void nlocker_lock(nlocker_t * locker) {
    if( locker->type == NLOCKER_THREAD ) {
        pthread_mutex_lock(locker->mutex);
    }
}

void nlocker_unlock(nlocker_t * locker) {
    if( locker->type == NLOCKER_THREAD ) {
        pthread_mutex_unlock(locker->mutex);
    }
}