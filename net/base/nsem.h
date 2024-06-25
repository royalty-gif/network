#ifndef _NSEM_H
#define _NSEM_H

#include <pthread.h>

typedef struct _sys_sem_t {
    int count;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
} * sys_sem_t;

sys_sem_t sys_sem_create(int init_count);
void sys_sem_free(sys_sem_t sem);
int sys_sem_wait(sys_sem_t sem, size_t tmo_ms);
void sys_sem_notify(sys_sem_t sem);

#endif