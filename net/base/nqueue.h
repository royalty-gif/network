#ifndef _NQUEUE_H_
#define _NQUEUE_H_

#include "nlist.h"

typedef struct {
    nlist_node_t head;
    int length;
} nqueue_t;

typedef nlist_node_t nqueue_node_t;

void nqueue_init(nqueue_t* queue);
void nqueue_node_init(nqueue_node_t* node);
int nqueue_is_empty(nqueue_t* queue);
void nqueue_push(nqueue_t* queue, nqueue_node_t* node);
nqueue_node_t* nqueue_top(nqueue_t* queue);
nqueue_node_t* nqueue_pop(nqueue_t* queue);
int nquene_length(nqueue_t* queue);

#endif