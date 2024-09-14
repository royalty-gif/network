/*
 *  @brief: pcap虚拟网络接口
 */

#ifndef _NETIF_PCAP_H_
#define _NETIF_PCAP_H_

#include "netif.h"

// pcap网络接口的驱动数据
typedef struct _pcap_data_t {
    const char* ip;                    // 使用的网卡
    const uint8_t* hwaddr;             // 网卡的mac地址
}pcap_data_t;

extern const netif_ops_t netdev_ops;

#endif