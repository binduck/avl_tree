#ifndef _AVL_PRIVATE_H
#define _AVL_PRIVATE_H

#include <stdbool.h>
#include <stdlib.h>

#define LEFT(node) (node->childs[0])
#define RIGHT(node) (node->childs[1])

typedef struct node {
    long value;
    long quantity;
    struct node *childs[2];
    long _subtree_height;
} node_t;

struct tree {
    node_t *head;
};

#ifdef __cplusplus
extern "C"
{
#endif

node_t *create_node(long value);
void _add_aux(node_t **target, node_t *new_node);
void add_track_link(node_t *head, node_t *new_node, node_t **parent_link);
void add(node_t *head, long value, node_t **parent_link);
void del(node_t *head, long value);
bool exists(node_t *head, long value);
void rebalance(node_t *node, node_t **parent_link);
unsigned long measure(node_t *head);
unsigned long right_height(node_t *head);
unsigned long left_height(node_t *head);
unsigned short get_balance_factor(node_t *head);
void rotate_right(node_t *head, node_t **parent_link);
void rotate_left(node_t *head, node_t **parent_link);
void delete_recursive(node_t *head);
void delete_node(node_t *head);

#ifdef __cplusplus
}
#endif

#endif
