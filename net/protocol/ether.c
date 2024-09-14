#include "ether.h"
#include "log.h"

static net_err_t ether_open(struct _netif_t *netif) {

}

static void ether_close(struct _netif_t *netif) {

}

static net_err_t ether_in(struct _netif_t* netif, pktbuf_t* buf) {

}

net_err_t ether_out(struct _netif_t *netif, ipaddr_t *dest, pktbuf_t *buf) {
    
}

net_err_t ether_init(void) {
    static const link_layer_t link_layer = {
        .type = NETIF_TYPE_ETHER,
        .open = ether_open,
        .close = ether_close,
        .in = ether_in,
        .out = ether_out,
    };

    info("init ether begin!");

    net_err_t err = netif_register_layer(NETIF_TYPE_ETHER, &link_layer);
    if( err != NET_ERR_OK) {
        error("netif register layer error, err = %d", err);
        return err;
    }

    info("init ether end!");

    return NET_ERR_OK;
}