#include "nlist.h"
#include <stdio.h>

void nlist_init(nlist_node_t *list) {
    list->next = list;
    list->prev = list;
}

static void nlist_insert_node(nlist_node_t* node, nlist_node_t* prev, nlist_node_t* next) {
    next->prev = node;
    node->next = next;
    prev->next = node;
    node->prev = prev;
}

void nlist_insert_after(nlist_t *list, nlist_node_t *node) {
    nlist_insert_node(node, list, list->next);
}

void nlist_insert_before(nlist_t *list, nlist_node_t *node) {
    nlist_insert_node(node, list->prev, list);
}

static void nlist_remove_node(nlist_node_t* prev, nlist_node_t* next) {
    prev->next = next;
    next->prev = prev;
}

void nlist_remove_init(nlist_node_t* node) {
    nlist_remove_node(node->prev, node->next);

    nlist_init(node);
}

void nlist_remove_entry(nlist_node_t* node) {
    nlist_remove_node(node->prev, node->next);
}

void nlist_remove(nlist_node_t *node) {
    nlist_remove_node(node->prev, node->next);

    node->next = NULL;
    node->prev = NULL;
}

int nlist_is_empty(nlist_t *list) {
    nlist_node_t* next = list->next;

    return is_equal(next, list) && is_equal(next, list->prev);
}