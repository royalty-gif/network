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
    return nlist_is_empty(&queue->head);
}

void nqueue_push_front(nqueue_t *queue, nqueue_node_t *node) {
    nlist_insert_after(&queue->head, node);

    queue->length++;
}

void nqueue_push_back(nqueue_t* queue, nqueue_node_t* node) {
    nlist_insert_before(&queue->head, node);

    queue->length++;
}

nqueue_node_t *nqueue_front(nqueue_t *queue) {
    return queue->head.next;
}

nqueue_node_t* nqueue_back(nqueue_t* queue) {
    return queue->head.prev;
}

static nqueue_node_t *nqueue_pop(nqueue_t *queue, int front) {
    nqueue_node_t* ret = NULL;

    if( queue->length > 0 ) {
        ret = front ? queue->head.next : queue->head.prev;

        nlist_remove(ret);

        queue->length--;
    }

    return ret;
}

nqueue_node_t *nqueue_pop_front(nqueue_t *queue) {
    return nqueue_pop(queue, 1);
}

nqueue_node_t *nqueue_pop_back(nqueue_t *queue) {
    return nqueue_pop(queue, 0);
}

int nquene_length(nqueue_t *queue) {
    return queue->length;
}