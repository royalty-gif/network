/*
 *  @brief: IP地址定义及接口函数
 */

#ifndef _IDADDR_H_
#define _IDADDR_H_

#include <stdint.h>
#include "net_err.h"

#define IPV4_ADDR_BROADCAST  0xFFFFFFFF  // 广播地址
#define IPV4_ADDR_SIZE       4           // IPv4地址长度

// IP地址
typedef struct _ipaddr_t {
    enum {
        IPADDR_V4,
    } type;

    union {
        // IP地址总是按照大端存放
        uint32_t q_addr;                  // 32位整体描述
        uint8_t a_addr[IPV4_ADDR_SIZE];   // 数组描述
    };
} ipaddr_t;

void ipaddr_set_any(ipaddr_t* ip);
void ipaddr_set_loop(ipaddr_t* ipaddr);
ipaddr_t *ipaddr_get_any(void);
net_err_t ipaddr_from_str(ipaddr_t * dest, const char* str);
net_err_t ipaddr_copy(ipaddr_t * dest, const ipaddr_t * src);
ipaddr_t ipaddr_get_net(const ipaddr_t * ipaddr, const ipaddr_t * netmask);
void ipaddr_set_all_one(ipaddr_t* ip);

#endif