#include <stdio.h>
#include <unistd.h>
#include <malloc.h>
#include "ether.h"
#include "net_err.h"
#include "netif.h"
#include "nqueue.h"
#include "pcap.h"
#include "log.h"
#include "exmsg.h"
#include "utility.h"
#include "pktbuf.h"
#include "netif_pcap.h"
#include "netif.h"

static void pcap_test_example(void) {
    pcap_t *handle;
    char errbuf[PCAP_ERRBUF_SIZE];
    const char *filename = "output.pcap";
    const char *device = "eth0";
    const int packet_cnt = 100;

    /* 打开设备 */
    handle = pcap_open_live(device, 65535, 1, 0, errbuf);
    if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", "eth0", errbuf);
		return ;
    }

    /* 创建 pcap_dumper_t 用于保存数据 */
    pcap_dumper_t *dumpfile = pcap_dump_open(handle, filename);
    if (dumpfile == NULL) {
		fprintf(stderr, "Couldn't open dump file %s: %s\n", filename,
        		pcap_geterr(handle));
        return ;
    }

    /* 进入捕获循环 */
    struct pcap_pkthdr header;
    const u_char *packet;
    int cnt = 0;

    while (((packet = pcap_next(handle, &header)) != NULL) &&
            (cnt < packet_cnt)) {
        /* 将数据包写入文件 */
		pcap_dump((u_char*)dumpfile, &header, packet);
		cnt += 1;
    }

    /* 关闭资源 */
    pcap_close(handle);
    pcap_dump_close(dumpfile);

}

// 网络设备初始化
static net_err_t netdev_init(void) {
    pcap_data_t netdev0_data = { .ip = netdev0_phy_ip, .hwaddr = netdev0_hwaddr };

    // 打开网络接口
    netif_t* netif = netif_add("netif 0", &netdev_ops, &netdev0_data);
    if( !netif ) {
        error("netif add failed!");
        return NET_ERR_SYS;
    }

    // 设置地址
    ipaddr_t ip, mask, gw;
    ipaddr_from_str(&ip, netdev0_ip);
    ipaddr_from_str(&mask, netdev0_mask);
    ipaddr_from_str(&gw, netdev0_gw);
    netif_set_addr(netif, &ip, &mask, &gw);

    return NET_ERR_OK;
}

static net_err_t test_exmsg_func(struct _func_msg_t* msg) {
    info("test exmsg func!");

    return NET_ERR_OK;
}

static void exmsg_test_example(void) {
    exmsg_init();

    exmsg_start();

    sleep(1);

    exmsg_func_exec(test_exmsg_func, NULL);

    sleep(1);
}

static void nqueue_test_example(void) {
    nqueue_t queue;
    nqueue_init(&queue);

    typedef struct _nqueue_node_t {
        nlist_node_t node;
        int data;
    } nqueue_node;

    int test_count = 10;

    int push_front = 1;
    for(int i = 0; i < test_count; i++) {
        nqueue_node* qnode = (nqueue_node*)malloc(sizeof(nqueue_node));
        qnode->data = i;

        if( push_front ) {
            nqueue_push_front(&queue, &qnode->node);
        } else {
            nqueue_push_back(&queue, &qnode->node);
        }
    }

    info("queue length: %d", nquene_length(&queue));
    info("front: %d, back: %d", 
         container_of(nqueue_front(&queue),nqueue_node, node)->data,
         container_of(nqueue_back(&queue),nqueue_node, node)->data);

    int pop_front = 1;
    for(int i = 0; i < test_count; i++) {
        nqueue_node_t* node = NULL;
        if( pop_front ) {
            node = nqueue_pop_front(&queue);
        } else {
            node = nqueue_pop_back(&queue);
        }

        nqueue_node* qnode = container_of(node, nqueue_node, node);
        info("node value: %d", qnode->data);
        free(qnode);
    }
}

static void pktbuf_test_example(void) {
    pktbuf_init();

    pktbuf_t* pbuf = pktbuf_alloc(65535);
    if( pbuf == NULL ) {
        info("pbuf alloc failed!");
    }

    info("add header!");
    pktbuf_add_header(pbuf, 10000, 0);

    info("remove header!");
    pktbug_remove_header(pbuf, 10000);
}

int main() { 
    if(log_init() != LOG_NO_ERROR) {
        printf("log init error\n");
        return -1;
    }

    exmsg_init();
    pktbuf_init();
    netif_init();
    ether_init();

    netdev_init();

    // 启动
    exmsg_start();

    while(1);

    log_fini();

    return 0;
}
