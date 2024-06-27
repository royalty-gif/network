#include "nqueue.h"
#include "nlist.h"
#include <stdio.h>

void nqueue_init(nqueue_t *queue) {
    nlist_init((nlist_node_t*)queue);
    queue->length = 0;
}

void nqueue_node_init(nqueue_node_t *node) {
    nlist_init((nlist_node_t*)node);
}

int nqueue_is_empty(nqueue_t *queue) {
    return nlist_is_empty((nlist_node_t*)queue);
}

void nqueue_push(nqueue_t *queue, nqueue_node_t *node) {
    nlist_insert_before((nlist_t*)queue, node);

    queue->length++;
}

nqueue_node_t *nqueue_top(nqueue_t *queue) {
    return queue->head.next;
}

nqueue_node_t *nqueue_pop(nqueue_t *queue) {
    nqueue_node_t* ret = NULL;

    if( queue->length > 0 ) {
        ret = queue->head.next;

        nlist_remove(ret);

        queue->length--;
    }

    return ret;
}

int nquene_length(nqueue_t *queue) {
    return queue->length;
}