/*
 *  @brief: 网络接口层
 */

#ifndef _NETIF_H_
#define _NETIF_H_

#include <stdint.h>
#include "fixq.h"
#include "ipaddr.h"
#include "net_cfg.h"
#include "net_err.h"
#include "nlist.h"
#include "pktbuf.h"

// 硬件地址
typedef struct _netif_hwaddr_t {
    uint8_t len;
    uint8_t addr[NETIF_HWADDR_SIZE];
} netif_hwaddr_t;

// 网络接口类型
typedef enum _netif_type_t {
    NETIF_TYPE_NONE = 0,    // 无效接口
    NETIF_TYPE_ETHER,       // 以太网接口
    NETIF_TYPE_LOOP,        // 回环接口

    NETIF_TYPE_SIZE,
} netif_type_t;

// 网络接口支持的操作
struct _netif_t;
typedef struct _netif_ops_t {
    net_err_t (*open)(struct _netif_t *netif, void* data);
    void (*close)(struct _netif_t* netif);
    net_err_t (*xmit)(struct _netif_t* netif);
} netif_ops_t;

// 链路层处理接口
typedef struct _link_layer_t {
    netif_type_t type;

    net_err_t (*open)(struct _netif_t* netif);
    void (*close)(struct _netif_t* netif);
    net_err_t (*in)(struct _netif_t* netif, pktbuf_t* buf);
    net_err_t (*out)(struct _netif_t* netif, ipaddr_t* dest, pktbuf_t* buf);
} link_layer_t;

// 网络接口
typedef struct _netif_t {
    char name[NETIF_NAME_SIZE];   // 网络接口名称

    netif_hwaddr_t hwaddr;  // 硬件地址
    ipaddr_t ipaddr;        // ip地址
    ipaddr_t netmask;       // 子网掩码
    ipaddr_t gateway;       // 网关地址

    enum {                  // 接口状态
        NETIF_CLOSED,       // 已关注
        NETIF_OPENED,       // 已经打开
        NETIF_ACTIVE,       // 激活状态
    }state;

    netif_type_t type;      // 网络接口类型
    int mtu;                // 最大传输单元

    const netif_ops_t* ops; // 驱动类型
    void* ops_data;         // 底层私有数据

    const link_layer_t* link_layer; // 链路层结构

    nlist_node_t node;      // 链表节点，用于多个链接网络接口

    fixq_t in_q;            // 输入队列
    void *in_q_buf[NETIF_INQ_SIZE];
    fixq_t out_q;           // 输出队列
    void *out_q_buf[NETIF_OUTQ_SIZE];

} netif_t;

net_err_t netif_init(void);
netif_t* netif_add(const char* dev_name, const netif_ops_t* ops, void* ops_data);
net_err_t netif_remove(netif_t* netif);
net_err_t netif_register_layer(netif_type_t type, const link_layer_t* layer);
void netif_set_hwaddr(netif_t* netif, const uint8_t* hwaddr, int len);

// 输入输出管理
net_err_t netif_put_in(netif_t* netif, pktbuf_t* buf, int tmo);
pktbuf_t* netif_get_in(netif_t* netif, int tmo);
net_err_t netif_put_out(netif_t * netif, pktbuf_t * buf, int tmo);
pktbuf_t* netif_get_out(netif_t * netif, int tmo);
#endif