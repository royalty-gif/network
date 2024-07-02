#ifndef _SYS_PLAT_H_
#define _SYS_PLAT_H_

#include <pthread.h>

// 线程相关
typedef pthread_t sys_thread_t;

typedef void* (*sys_thread_func_t)(void* arg);
sys_thread_t sys_thread_create(sys_thread_func_t entry, void* arg);
void sys_thread_destroy(int err);
sys_thread_t sys_thread_self(void);

void sys_sleep(int ms);

#endif