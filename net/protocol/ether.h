/**
 * @brief: 以太网协议
 */

#ifndef _ETHER_H_
#define _ETHER_H_

#include <stdint.h>
#include "net_err.h"
#include "netif.h"
#include "pktbuf.h"
 
#define ETH_MTU         1500   // 最大传输单元，数据区的大小
#define ETH_MAC_SIZE    6      // mac地址长度
#define ETH_MIN_SIZE    46     // payload最小长度，即头部+46

#pragma pack(1)

/**
 *  @brief: 以太网帧头
 */
typedef struct _ether_head_t {
    uint8_t dest[ETH_MAC_SIZE];    // 目标mac地址
    uint8_t src[ETH_MAC_SIZE];     // 源mac地址
    uint16_t protocol;             // 协议/长度
} ether_head_t;

/**
 *  @brief: 以太网帧格式
 */
typedef struct _ether_pkt_t {
    ether_head_t head;             // 帧头
    uint8_t payload[ETH_MTU];      // 有效负载
} ether_pkt_t;

#pragma pack()


net_err_t ether_init(void);

#endif