#include "fixq.h"
#include "net_err.h"
#include "log.h"
#include "nlocker.h"
#include "nsem.h"

/**
 * @brief: 初始化定长消息队列
 */
net_err_t fixq_init(fixq_t * q, void** buf, int size, nlocker_type_t type) {
    // 创建锁   
    net_err_t ret = nlocker_init(&q->locker, type);
    if( ret != NET_ERR_OK ) {
        error("init locker failed!");
        goto locker_failed;
    }

    // 创建信号量
    q->send_sem = sys_sem_create(size);
    if( q->send_sem == SYS_SEM_INVALID ) {
        error("create send sem failed!");
        ret = NET_ERR_SYS;
        goto send_sem_failed;
    }

    q->recv_sem = sys_sem_create(0);
    if( q->recv_sem == SYS_SEM_INVALID ) {
        error("create recv sem failed!");
        ret = NET_ERR_SYS;
        goto recv_sem_failed;
    }

    q->size = size;
    q->buf = buf;
    q->in = q->out = q->cnt = 0;

    return ret;

recv_sem_failed:
    sys_sem_free(q->send_sem);
    q->send_sem = SYS_SEM_INVALID;
send_sem_failed:
    nlocker_destroy(&q->locker);
locker_failed:
    return ret;
}

/**
 *  @brief: 往队列中发送消息
 */
net_err_t fixq_send(fixq_t* q, void* msg, int tmo) {
    nlocker_lock(&q->locker);
    if( (q->cnt >= q->size) && (tmo < 0) ) {
        // 缓存满且不需要等待，则退出
        info("fix queue is full!");
        nlocker_unlock(&q->locker);

        return NET_ERR_FULL;
    }
    nlocker_unlock(&q->locker);

    // 等待数据包可用
    if( sys_sem_wait(q->send_sem, tmo) < 0 ) {
        error("wait send sem failed!");
        return NET_ERR_TMO;
    }

    // 写入缓存
    nlocker_lock(&q->locker);
    q->buf[q->in++] = msg;
    if( q->in >= q->size ) {
        q->in = 0;
    }
    q->cnt++;
    nlocker_unlock(&q->locker);

    // 通知有消息可用
    sys_sem_notify(q->recv_sem);

    return NET_ERR_OK;
}

/**
 *  @brief: 从队列中接收消息
 */
void* fixq_recv(fixq_t* q, int tmo) {
    nlocker_lock(&q->locker);
    if( !q->cnt && tmo < 0 ) {
        // 缓存为空且不需要等待，则退出
        nlocker_unlock(&q->locker);
        return NULL;
    }

    nlocker_unlock(&q->locker);

    // 等待数据包可用
    if (sys_sem_wait(q->recv_sem, tmo) < 0) {
        //error("wait send sem failed!");
        return NULL;
    }

    // 从缓存读出数据
    nlocker_lock(&q->locker);
    void* msg = q->buf[q->out++];
    if( q->out >= q->size ) {
        q->out = 0;
    }
    q->cnt--;
    nlocker_unlock(&q->locker);

    // 通知有空余空间可用
    sys_sem_notify(q->send_sem);

    return msg;
}

/**
 *  @brief: 销毁队列
 */
void fixq_destroy(fixq_t * q) {
    nlocker_destroy(&q->locker);
    sys_sem_free(q->send_sem);
    sys_sem_free(q->recv_sem);
}

int fixq_count(fixq_t *q) {
    nlocker_lock(&q->locker);
    int count = q->cnt;
    nlocker_unlock(&q->locker);
    
    return count;
}