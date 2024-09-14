#include "ipaddr.h"
#include "net_err.h"

// 设置ip为任意ip地址
void ipaddr_set_any(ipaddr_t* ip) {
    ip->q_addr = 0;
}

// 设置环回接口地址
void ipaddr_set_loop(ipaddr_t* ipaddr) {
    ipaddr->a_addr[0] = 127;
    ipaddr->a_addr[1] = 0;
    ipaddr->a_addr[2] = 0;
    ipaddr->a_addr[3] = 1;
}

// 获取缺省地址
ipaddr_t* ipaddr_get_any(void) {
    static ipaddr_t ipaddr_any = { .q_addr = 0 };
    return &ipaddr_any;
}

// 将字符串转换成ipaddr_t格式
net_err_t ipaddr_from_str(ipaddr_t* dest, const char* str) {
    if( !dest || !str ) {
        return NET_ERR_PARAM;
    } 

    dest->q_addr = 0;

    // 不断扫描字符串，直到字符串结束
    char c;
    uint8_t* p = dest->a_addr;
    uint8_t subAddr = 0;

    while( (c = *str++) != '\0' ) {
        if( ('0' <= c) && (c <= '9') ) {
            // 是数字字符，进行合并计算
            subAddr = subAddr * 10 + c - '0';
        } else if( c == '.' ) {
            // .分隔符，表示进入下一个地址符，重新计算
            *p++ = subAddr;
            subAddr = 0;
        } else {
            // 参数错误
            return NET_ERR_PARAM;
        }
    }
    
    // 写入最后的值
    *p++ = subAddr;
    return NET_ERR_OK;
}

net_err_t ipaddr_copy(ipaddr_t * dest, const ipaddr_t * src) {
    if( !dest || !src ) {
        return NET_ERR_PARAM;
    }

    dest->q_addr = src->q_addr;
    return NET_ERR_OK;
}

ipaddr_t ipaddr_get_net(const ipaddr_t * ipaddr, const ipaddr_t * netmask) {
    ipaddr_t netId = {};

    if( ipaddr && netmask ) {
        netId.q_addr = ipaddr->q_addr & netmask->q_addr;
    }
    
    return netId;
}

void ipaddr_set_all_one(ipaddr_t* ip) {
    if( ip ) {
        ip->q_addr = 0xFFFFFFFF;
    }
}