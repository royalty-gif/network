#ifndef  _NLIST_H_
#define  _NLIST_H_

#include "utility.h"

typedef struct nlist_node_s {
    struct nlist_node_s* next;
    struct nlist_node_s* prev;
} nlist_node_t;

typedef nlist_node_t nlist_t;

#define list_for_each(node, list) \
    for (node = (list)->next; !is_equal(node, list); node = node->next)

#define list_entry(ptr, type, member) container_of(ptr, type, member)

void nlist_init(nlist_node_t* list);
void nlist_insert_after(nlist_t* list, nlist_node_t* node);
void nlist_insert_before(nlist_t* list, nlist_node_t* node);
void nlist_remove(nlist_node_t* node);
int nlist_is_empty(nlist_t* list);

#endif