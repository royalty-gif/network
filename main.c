#include <stdio.h>
#include <unistd.h>
#include "net_err.h"
#include "pcap.h"
#include "log.h"
#include "exmsg.h"

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

int main() { 
    if(log_init() != LOG_NO_ERROR) {
        printf("log init error\n");
        return -1;
    }

    exmsg_test_example();

    log_fini();

    return 0;
}
