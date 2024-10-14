#include "timer.h"
#include "net_cfg.h"
#include "net_err.h"
#include "nlist.h"
#include "log.h"
#include "sys_plat.h"

static nlist_t timer_list;    // 当前软定时器列表

#define NET_MAX_TIMEOUT  0x7fffffff
#define TIME_LESS_THAN(t, compare_to) ( (((uint32_t)((t)-(compare_to))) > NET_MAX_TIMEOUT) ? 1 : 0 )

net_err_t net_timer_init(void) {
    info("timer init");

    nlist_init(&timer_list);

    info("init done");
    return NET_ERR_OK;
}

/**
 *  @brief: 将结点按超时时间从小到达插入进链表中
 */
static void insert_timer(net_timer_t* timer) {
    nlist_node_t* node, *next;

    if( nlist_is_empty(&timer_list) ) {
        nlist_insert_after(&timer_list, &timer->node);
    } else {
        nlist_for_each_safe(node, next, &timer_list) {
            net_timer_t* curr = nlist_entry(node, net_timer_t, node);

            if( timer->time > curr->time ) {
                continue;
            } else if( timer->time == curr->time ) {
                nlist_insert_after(&curr->node, &timer->node);
                break;
            } else {
                nlist_insert_before(&curr->node, &timer->node);
                break;
            }
        }
    }
}

net_err_t net_timer_add(net_timer_t* timer, const char* name, timer_proc_t proc, void* arg, int ms, int flag) {
    info("insert timer: %s", name);

    plat_strncpy(timer->name, name, TIMER_NAME_SIZE);
    timer->name[TIMER_NAME_SIZE - 1] = '\n';
    timer->reload = ms;
    timer->time = sys_now() + timer->reload;
    timer->proc = proc;
    timer->arg = arg;
    timer->flag = flag;
    nlist_init(&timer->node);

    // 插入链表中
    insert_timer(timer);

    return NET_ERR_OK;
}

void net_timer_remove(net_timer_t* timer) {
    if( nlist_is_empty(&timer_list) ) {
        error("timer list is empty!");
        return;
    }

    nlist_node_t* node, *next;
    nlist_for_each_safe(node, next, &timer_list) {
        net_timer_t* curr = nlist_entry(node, net_timer_t, node);
        if( curr != timer ) {
            continue;
        } else {
            nlist_remove(&timer->node);
            break;
        }
    }
}

net_err_t net_timer_check_tmo(void) {
    // 超时待处理的链表
    nlist_t timeout_list;
    nlist_init(&timeout_list);

    uint32_t now = sys_now();

    // 遍历超时事件
    nlist_node_t* node, *next;
    nlist_for_each_safe(node, next, &timer_list) {
        net_timer_t* curr = nlist_entry(node, net_timer_t, node);
        if( TIME_LESS_THAN(now, curr->time) ) {
            break;
        }

        nlist_remove_init(&curr->node);
        nlist_insert_after(&timeout_list, &curr->node);
    }

    // 处理超时事件
    nlist_for_each_safe(node, next, &timeout_list) {
        net_timer_t* timer = nlist_entry(node, net_timer_t, node);

        // 执行调用
        timer->proc(timer, timer->arg);

        // 重载定时器
        if( timer->flag & NET_TIMER_RELOAD ) {
            info("reload timer!");
            timer->time = now + timer->reload;
            insert_timer(timer);
        }
    }

    return NET_ERR_OK;
}