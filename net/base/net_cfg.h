#ifndef _NET_CFG_H_
#define _NET_CFG_H_

#define NETIF_USE_INT  0     // 网络接口使用中断保护
#define EXMSG_MSG_CNT  10    // 消息缓冲区大小

#define PKTBUF_BLK_SIZE 1024 // 数据包块大小
#define PKTBUF_BLK_CNT 2048  // 数据包块的数量
#define PKTBUF_BUF_CNT 1024  // 数据包的数量

#define NETIF_HWADDR_SIZE  10  // 硬件地址长度，mac地址最少6个字节
#define NETIF_NAME_SIZE    10  // 网络接口名称大小
#define NETIF_DEV_CNT      4   // 网络接口的数量
#define NETIF_INQ_SIZE     50  // 网卡输入队列最大容量
#define NETIF_OUTQ_SIZE    50  // 网卡输出队列最大容量
#endif