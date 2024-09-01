#include "ipaddr.h"

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