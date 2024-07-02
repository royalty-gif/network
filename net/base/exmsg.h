#ifndef _EXMSG_H_
#define _EXMSG_H_

#include "sys_plat.h"
#include "net_err.h"
#include "nsem.h"

struct _func_msg_t;
typedef net_err_t (*exmsg_func_t)(struct _func_msg_t* msg);

/**
 *  工作函数调用
 */
typedef struct _func_msg_t {
    sys_thread_t thread;
    exmsg_func_t func;
    void* param;
    net_err_t err;

    sys_sem_t wait_sem;

} func_msg_t;


/**
 *  @brief: 传递给核心线程的消息
 */
typedef struct _exmsg_t {
    // 消息类型
    enum {
        NET_EXMSG_NETIF_IN,             // 网络接口数据消息
        NET_EXMSG_FUN,                  // 工作函数调用
    }type;

    // 消息数据
    union {
        func_msg_t* func;               // 工作函数调用消息
    };
} exmsg_t;

net_err_t exmsg_init(void);
net_err_t exmsg_start(void);

net_err_t exmsg_func_exec(exmsg_func_t func, void* param);

#endif