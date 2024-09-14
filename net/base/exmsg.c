#include "exmsg.h"
#include "net_cfg.h"
#include "fixq.h"
#include "mblock.h"
#include "net_err.h"
#include "log.h"
#include "nsem.h"

static void* msg_tbl[EXMSG_MSG_CNT];      // 消息缓冲区
static fixq_t msg_queue;                  // 消息队列
static exmsg_t msg_buffer[EXMSG_MSG_CNT]; // 消息块
static mblock_t msg_block;                // 消息分配器

net_err_t exmsg_init(void) {
    info("exmsg init!");

    net_err_t ret = mblock_init(&msg_block, msg_buffer, sizeof(exmsg_t), EXMSG_MSG_CNT, NLOCKER_THREAD);
    if( ret != NET_ERR_OK ) {
        error("mblock init failed!");
        goto mblock_failed;
    }

    ret = fixq_init(&msg_queue, msg_tbl, EXMSG_MSG_CNT, NLOCKER_THREAD);
    if( ret != NET_ERR_OK ) {
        error("fixq init failed!");
        goto fixq_failed;
    }

    info("exmsg done!");

    return ret;

fixq_failed:
    mblock_destroy(&msg_block);
mblock_failed:
    return ret;
}

/**
 *  @brief: 内部函数的执行
 */
static void do_func(func_msg_t* func_msg) {
    info("calling func!");

    func_msg->err = func_msg->func(func_msg->param);

    sys_sem_notify(func_msg->wait_sem);

    info("func exec complete!");
}

/**
 *  @brief: 网络接口有数据到达时的处理
 */
static void do_netif_in(exmsg_t* msg) {
    netif_t* netif = msg->netif.netif;

    // 反复从接口中取出包，然后一次性处理
    pktbuf_t* buf;
    while ((buf = netif_get_in(netif, -1))) {
        info("recv a packet!");
        pktbuf_free(buf);
    }
}

static void* work_thread(void* arg) {

    info("The exmsg work thread is running ...");

    while(1) {
        exmsg_t* msg = (exmsg_t*)fixq_recv(&msg_queue, 0);

        if( msg ) {
            info("recvive a msg, type: %d", msg->type);

            switch(msg->type) {
                case NET_EXMSG_NETIF_IN: {
                    do_netif_in(msg);
                    break;
                }

                case NET_EXMSG_FUN: {
                    do_func(msg->func);
                    break;
                }
            }

            // 释放消息
            mblock_free(&msg_block, msg);
        }
    }

    info("The exmsg work thread is exit!");
    return NULL;
}

net_err_t exmsg_start(void) {
    sys_thread_t thread = sys_thread_create(work_thread, NULL);
    if( thread == SYS_THREAD_INVALID ) {
        error("create thread failed!");
        return NET_ERR_SYS;
    }

    return NET_ERR_OK;
}

/**
 *  @brief: 执行内部工作函数调用
 */
net_err_t exmsg_func_exec(exmsg_func_t func, void *param) {
    net_err_t ret = NET_ERR_OK;
    func_msg_t func_msg;

    func_msg.wait_sem = sys_sem_create(0);
    if( func_msg.wait_sem == SYS_SEM_INVALID ) {
        error("create sem failed!");
        goto sem_failed;
    }

    // 构造消息
    func_msg.thread = sys_thread_self();
    func_msg.func = func;
    func_msg.param = param;
    func_msg.err = NET_ERR_OK;

    // 分配消息结构
    exmsg_t* msg = (exmsg_t*)mblock_alloc(&msg_block, 0);
    if( msg == NULL ) {
        error("alloc msg failed!");
        goto mblock_failed;
    } 

    msg->type = NET_EXMSG_FUN;
    msg->func = &func_msg;

    info("begin call func!");
    ret = fixq_send(&msg_queue, msg, 0);
    if( ret != NET_ERR_OK ) {
        error("send msg failed!");
        goto fixq_failed;
    }

    // 等待执行完成
    sys_sem_wait(func_msg.wait_sem, 0);
    info("end call func!");

    // 释放信号量
    sys_sem_free(func_msg.wait_sem);

    return func_msg.err;

fixq_failed:
    mblock_free(&msg_block, msg);
mblock_failed:
    sys_sem_free(func_msg.wait_sem);
sem_failed:
    return ret;
}


/**
 *  @brief: 收到来自网卡的消息
 */
net_err_t exmsg_netif_in(netif_t *netif) {
    // 分配一个消息结构
    exmsg_t* msg = mblock_alloc(&msg_block, -1);
    if (!msg) {
        error("no free exmsg");
        return NET_ERR_MEM;
    }

    // 写消息内容
    msg->type = NET_EXMSG_NETIF_IN;
    msg->netif.netif = netif;

    // 发送消息
    net_err_t err = fixq_send(&msg_queue, msg, -1);
    if (err < 0) {
        error("fixq full");
        mblock_free(&msg_block, msg);
        return err;
    }

    return NET_ERR_OK;
}