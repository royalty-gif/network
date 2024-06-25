#include "nsem.h"
#include <malloc.h>
#include <errno.h>
#include <pthread.h>
#include "net_err.h"
#include "log.h"
#include "utility.h"
sys_sem_t sys_sem_create(int init_count) {
    sys_sem_t sem = (sys_sem_t)malloc(sizeof(struct _sys_sem_t));
    if( !sem ) {
        error("sem create error!");
        goto malloc_failed;
    }

    sem->count = init_count;

    int err = pthread_cond_init(&sem->cond, NULL);
    if( err ) {
        error("sem cond init error!");
        goto cond_failed;
    }

    err = pthread_mutex_init(&sem->mutex, NULL);
    if( err ) {
        error("sem mutex init error!");
        goto mutex_failed;
    }

    return sem;

mutex_failed:
    pthread_cond_destroy(&sem->cond);
cond_failed:
    free(sem);
malloc_failed:
    return SYS_SEM_INVALID;
}

void sys_sem_free(sys_sem_t sem) {
    pthread_cond_destroy(&(sem->cond));
    pthread_mutex_destroy(&(sem->mutex));
    free(sem);
}

int sys_sem_wait(sys_sem_t sem, size_t tmo_ms) {

    struct timespec start_tm;
	struct timespec end_tm;

    clock_gettime(CLOCK_REALTIME, &start_tm);
    end_tm = ns_to_tm(tm_to_ns(start_tm) + tmo_ms * NSECS_PER_MSEC);
    
    pthread_mutex_lock(&(sem->mutex));

    if( sem->count <= 0 ) {
        int ret;

        if( tmo_ms > 0) {
            ret = pthread_cond_timedwait(&(sem->cond), &(sem->mutex), &end_tm);
            if (ret == ETIMEDOUT) {
                error("sem wait timeout!");
                goto failed;
            }
        } else {
            ret = pthread_cond_wait(&(sem->cond), &(sem->mutex));
            if (ret < 0) {
                error("sem wait failed");
                goto failed;
            }
        }
    }

    sem->count--;
    pthread_mutex_unlock(&(sem->mutex));

    return 0;

failed:
    pthread_mutex_unlock(&(sem->mutex));
    return NET_ERR_SYS;
}

void sys_sem_notify(sys_sem_t sem) {
    pthread_mutex_lock(&(sem->mutex));

    sem->count++;

    pthread_cond_signal(&(sem->cond));

    pthread_mutex_unlock(&(sem->mutex));
}