#include "netif.h"
#include "fixq.h"
#include "log.h"
#include "mblock.h"
#include "net_cfg.h"
#include "net_err.h"
#include "nlist.h"
#include "nlocker.h"
#include "sys_plat.h"
#include "ipaddr.h"

static netif_t netif_buffer[NETIF_DEV_CNT];     // 网络接口的数量
static mblock_t netif_mblock;                   // 接口分配结构
static nlist_t netif_list;                      // 网络接口列表
static netif_t * netif_default;                 // 缺省的网络列表
static const link_layer_t* link_layers[NETIF_TYPE_SIZE];      // 链接层驱动表

net_err_t netif_init(void) {
    info("init netif!");

    nlist_init(&netif_list);
    mblock_init(&netif_mblock, netif_buffer, sizeof(netif_t), NETIF_DEV_CNT, NLOCKER_NONE);

    netif_default = (netif_t*)0;

    plat_memset((void *)link_layers, 0, sizeof(link_layers));

    info("init done!");

    return NET_ERR_OK;
}

// 获取链路层协议处理模块
static const link_layer_t* netif_get_layer(netif_type_t type) {

    if ((NETIF_TYPE_NONE < type) && (type < NETIF_TYPE_SIZE) ) {
        return link_layers[type];
    }

    return (link_layer_t*)0;
}

netif_t* netif_add(const char* dev_name, const netif_ops_t* ops, void* ops_data) {
    // 分配一个网络接口
    netif_t* netif = (netif_t*)mblock_alloc(&netif_mblock, -1);
    if( !netif ) {
        error("no netif");
        return (netif_t*)0;
    }

    // 设置各种缺省值
    ipaddr_set_any(&netif->ipaddr);
    ipaddr_set_any(&netif->netmask);
    ipaddr_set_any(&netif->gateway);
    netif->mtu = 0;
    netif->type = NETIF_TYPE_NONE;
    nlist_init(&netif->node);

    plat_strncpy(netif->name, dev_name, NETIF_NAME_SIZE);
    netif->name[NETIF_TYPE_SIZE - 1] = '\0';
    netif->ops = ops;
    netif->ops_data = (void*)ops_data;

    // 初始化接收队列
    net_err_t err = fixq_init(&netif->in_q, netif->in_q_buf, NETIF_INQ_SIZE, NETIF_USE_INT ? NLOCKER_INT : NLOCKER_THREAD);
    if( err < 0 ) {
        error("netif in_q init error!");
        goto in_fixq_failed;
    }

    // 初始化发送队列
    err = fixq_init(&netif->out_q, netif->out_q_buf, NETIF_INQ_SIZE, NETIF_USE_INT ? NLOCKER_INT : NLOCKER_THREAD);
    if( err < 0 ) {
        error("netif out_q init error!");
        goto out_fixq_failed;
    }

    // 打开设备，对硬件做进一步的设置
    err = ops->open(netif, ops_data);
    if( err < 0 ) {
        error("netif ops open error!");
        goto open_failed;
    }

    // 标记为open
    netif->state = NETIF_OPENED;

    // 作进一步的检查，避免驱动没有写好
    if( netif->type == NETIF_TYPE_NONE ) {
        error("netif type unknown!");
        goto type_err;
    }

    // 获取链路层接口
    netif->link_layer = netif_get_layer(netif->type);
    if( !netif->link_layer && (netif->type != NETIF_TYPE_LOOP) ) {
        error("no link layer!");
        goto link_layer_err;
    }

    // 插入链表中
    nlist_insert_after(&netif_list, &netif->node);

    return netif;

link_layer_err:
type_err:
    netif->ops->close(netif);
open_failed:
    fixq_destroy(&netif->out_q);
out_fixq_failed:
    fixq_destroy(&netif->in_q);
in_fixq_failed:
    mblock_free(&netif_mblock, netif);
    return (netif_t*)0;
}

net_err_t netif_remove(netif_t* netif) {
    // 必须先取消active状态才能关闭
    if( netif->state == NETIF_ACTIVE ) {
        error("netif(%s) is active, close failed!", netif->name);
        return NET_ERR_STATE;
    }

    // 关闭设备
    netif->ops->close(netif);
    netif->state = NETIF_CLOSED;

    // 释放netif结构
    nlist_remove(&netif->node);
    mblock_free(&netif_mblock, netif);

    return NET_ERR_OK;
}

net_err_t netif_register_layer(netif_type_t type, const link_layer_t* layer) {
    // 类型参数错误
    if( (type < NETIF_TYPE_NONE) || (type > NETIF_TYPE_SIZE) ) {
        error("type error!");
        return NET_ERR_PARAM;
    }

    // 已存在
    if( link_layers[type] ) {
        error("link layer(%d) exist!", type);
        return NET_ERR_EXIST;
    }

    link_layers[type] = layer;

    return NET_ERR_OK;
}