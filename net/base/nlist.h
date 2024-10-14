#ifndef  _NLIST_H_
#define  _NLIST_H_

#include "utility.h"

typedef struct nlist_node_s {
    struct nlist_node_s* next;
    struct nlist_node_s* prev;
} nlist_node_t;

typedef nlist_node_t nlist_t;

#define nlist_for_each(node, list) \
    for (node = (list)->next; !is_equal(node, list); node = node->next)

#define nlist_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

#define nlist_entry(ptr, type, member) container_of(ptr, type, member)

void nlist_init(nlist_node_t* list);
void nlist_insert_after(nlist_t* list, nlist_node_t* node);
void nlist_insert_before(nlist_t* list, nlist_node_t* node);
void nlist_remove(nlist_node_t* node);
void nlist_remove_entry(nlist_node_t *node);
void nlist_remove_init(nlist_node_t *node);
int nlist_is_empty(nlist_t* list);

#endif