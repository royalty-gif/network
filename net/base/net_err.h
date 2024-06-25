#ifndef _NET_CFG_H_
#define _NET_CFG_H_

/*
 * 错误码及其类型
 */

#define SYS_THREAD_INVALID (sys_thread_t)0
#define SYS_SEM_INVALID (sys_sem_t)0
#define SYS_MUTEX_INVALID (sys_mutex_t)0

typedef enum {
    NET_ERR_OK  = 0,
    NET_ERR_SYS = -1,
} net_err_t;

#endif