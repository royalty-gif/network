#ifndef _NQUEUE_H_
#define _NQUEUE_H_

#include "nlist.h"

typedef struct {
    nlist_node_t head;
    int length;
} nqueue_t;

typedef nlist_node_t nqueue_node_t;

// 双端队列
void nqueue_init(nqueue_t* queue);
void nqueue_node_init(nqueue_node_t* node);
int nqueue_is_empty(nqueue_t* queue);
void nqueue_push_front(nqueue_t* queue, nqueue_node_t* node);
void nqueue_push_back(nqueue_t* queue, nqueue_node_t* node);
nqueue_node_t* nqueue_front(nqueue_t* queue);
nqueue_node_t* nqueue_back(nqueue_t* queue);
nqueue_node_t* nqueue_pop_front(nqueue_t* queue);
nqueue_node_t* nqueue_pop_back(nqueue_t* queue);
int nquene_length(nqueue_t* queue);

#endif