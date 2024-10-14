/**
 *  @brief: 定时器管理
 */

#ifndef _TIMER_H_
#define _TIMER_H_

#include "net_cfg.h"
#include "nlist.h"
#include "net_err.h"
#include <stdint.h>

#define NET_TIMER_RELOAD    (1 << 0)  // 自动重载

/**
 *  @brief: 定时器处理函数
 */
struct _net_timer_t;
typedef void (*timer_proc_t)(struct _net_timer_t* timer, void* arg);

/**
 *  @brief: 定时器结构
 */

typedef struct _net_timer_t {
    char name[TIMER_NAME_SIZE];    // 定时器名称
    int flag;                      // 是否自动重载

    uint32_t time;                 // 当前超时值
    uint32_t reload;               // 重载的定时值

    timer_proc_t proc;             // 超时处理函数
    void* arg;                     // 处理函数参数
 
    nlist_node_t node;             // 定时器链表节点
} net_timer_t;


net_err_t net_timer_init(void);
net_err_t net_timer_add(net_timer_t* timer, const char* name, timer_proc_t proc, void* arg, int ms, int flag);
void net_timer_remove(net_timer_t* timer);
net_err_t net_timer_check_tmo(void);

#endif