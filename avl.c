#include <stdlib.h>
#include <Windows.h>
#include <stdio.h>
#include "avl.h"
#include "avl_private.h"
#include <signal.h>

#define TRACE_AVL_CREATE (true)
#define TRACE_AVL_ADD    (false)

BOOL WINAPI ADDAPI DllMain(HINSTANCE i, DWORD r, LPVOID _) {
    return TRUE;
}

static unsigned long _max(unsigned long a, unsigned long b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// #######################################################################  
// public function implementation
// #######################################################################  
ADDCALL tree_t avl_create() {
    tree_t tree;

    if (TRACE_AVL_CREATE) {
        //printf("avl_create\n");
    }

    tree = (tree_t)malloc(sizeof(*tree));
    tree->head = NULL;
    return tree;
}

ADDCALL void avl_add(tree_t avl_tree, long value) {
    if (TRACE_AVL_ADD) {
        //printf("avl_add(%p, %d);\n", avl_tree, value);
    }

    if (avl_tree == NULL) {
        return;
    } else if (avl_tree->head == NULL) {
        avl_tree->head = create_node(value);
    } else {
        add(avl_tree->head, value, &(avl_tree->head));
    }
}

ADDCALL bool avl_exists(tree_t avl_tree, long value) {
    if (avl_tree == NULL) {
        return false;
    } else if (avl_tree->head == NULL) {
        return false;
    } else {
        return exists(avl_tree->head, value);
    }
}

ADDCALL void ADDAPI avl_destroy(tree_t avl_tree) {
    if (avl_tree == NULL) {
        return;
    } else if (avl_tree->head == NULL) {
        free(avl_tree);
        return;
    } else {
        delete_recursive(avl_tree->head);
        free(avl_tree);
    }
}

// #######################################################################  
// private function implementation
// #######################################################################  
node_t *create_node(long value) {
    node_t *new_node;
    new_node = (node_t *)malloc(sizeof(*new_node));
    new_node->value = value;
    new_node->childs[0] = new_node->childs[1] = NULL; 
    new_node->_subtree_height = 1;
    new_node->quantity = 1;
    return new_node;
}

void _add_aux(node_t **target, node_t *new_node) {
    if (*target == NULL) {
        *target = new_node;
    } else if ((*target)->value == new_node->value) {
        (*target)->quantity++;
    } else {
        add_track_link(*target, new_node, target);
    }
}

void add_track_link(node_t *head, node_t *new_node, node_t **parent_link) {
    if (new_node->value >= head->value) {
        _add_aux(&RIGHT(head), new_node);
    } else {
        _add_aux(&LEFT(head), new_node);
    }
    rebalance(head, parent_link);
    head->_subtree_height = measure(head);
}

void add(node_t *head, long value, node_t **parent_link) {
    node_t *new_node;
    new_node = create_node(value);
    add_track_link(head, new_node, parent_link);
}

bool exists(node_t *head, long value) {
    if (head == NULL) {
        return false;
    } else if (value == head->value) {
        return true;
    } else if (value > head->value) {
        return exists(RIGHT(head), value);
    } else {
        return exists(LEFT(head), value);
    }
}

void rebalance(node_t *node, node_t **parent_link) {
    node_t *new_head; 

    new_head = node;
    //printf("rebalancing at %d, left height %d, right height: %d\n", node->value, left_height(node), right_height(node));
    //printf("current parent link is %p\n", parent_link);
    //printf("current parent link points to %d\n", (* parent_link)->value);
    if (right_height(node) > left_height(node) + 1) {
        //printf("too heavy right subtree\n");
        if (left_height(RIGHT(node)) > right_height(RIGHT(node))) {
            //printf("right subtree is left heavy\n");
            rotate_right(RIGHT(node), &RIGHT(node));
        }
        rotate_left(node, &new_head);
    } else if (left_height(node) > right_height(node) + 1) {
        //printf("too heavy left subtree\n");
        if (right_height(LEFT(node)) > left_height(LEFT(node))) {
            //printf("right subtree is right heavy\n");
            rotate_left(LEFT(node), &LEFT(node));
        }
        rotate_right(node, &new_head);
    }
    if (parent_link != NULL) {
        //printf("setting parent link to node %d\n", new_head->value);
        *parent_link = new_head;
    }
}

unsigned long measure(node_t *head) {
    return _max(right_height(head), left_height(head)) + 1;
}

unsigned long right_height(node_t *head) {
    if (RIGHT(head) == NULL) {
        return 0;
    } else {
        return RIGHT(head)->_subtree_height;
    }
}

unsigned long left_height(node_t *head) {
    if (LEFT(head) == NULL) {
        return 0;
    } else {
        return LEFT(head)->_subtree_height;
    }
}

unsigned short get_balance_factor(node_t *head) {
    short diff;

    diff = left_height(head) - right_height(head);
    return abs(diff);
}

void rotate_right(node_t *head, node_t **parent_link) {
    node_t *new_head;
    new_head = LEFT(head);
    if (parent_link != NULL) {
        *parent_link = new_head;
    }
    LEFT(head) = RIGHT(new_head);
    RIGHT(new_head) = head;
}

void rotate_left(node_t *head, node_t **parent_link) {
    node_t *new_head;
    new_head = RIGHT(head);
    if (parent_link != NULL) {
        *parent_link = new_head;
    }
    RIGHT(head) = LEFT(new_head);
    LEFT(new_head) = head;
}

void delete_recursive(node_t *head) {
    if (LEFT(head) != NULL) {
        delete_recursive(LEFT(head));
    }
    if (RIGHT(head) != NULL) {
        delete_recursive(RIGHT(head));
    }
    delete_node(head);
}

void delete_node(node_t *head) {
    free(head);
}

static void _aux_raw_extractor(node_t *head, tree_raw avl_raw, unsigned space) {
    tree_edge_t *cursor;
    unsigned renew_space = 100000;

    // TODO: bug with space decrament
    if (space < 2) {
        avl_raw->edges = (tree_edge_t *) realloc(avl_raw->edges,
                            sizeof(tree_edge_t) * renew_space);
        space = renew_space;
    }
    if (LEFT(head)) {
        cursor = &(avl_raw->edges[avl_raw->count_edges++]);
        cursor->u = head->value;
        cursor->v = LEFT(head)->value;
        _aux_raw_extractor(LEFT(head), avl_raw, --space);
    }
    if (RIGHT(head)) {
        cursor = &(avl_raw->edges[avl_raw->count_edges++]);
        cursor->u = head->value;
        cursor->v = RIGHT(head)->value;
        _aux_raw_extractor(RIGHT(head), avl_raw, --space);
    }
}

ADDCALL tree_raw ADDAPI avl_get_raw_data(tree_t avl_tree) { 
    tree_raw avl_raw;
    unsigned initial_space = 1000;

    avl_raw = (tree_raw) malloc(sizeof(*avl_raw));
    avl_raw->count_edges = 0;
    avl_raw->edges = (tree_edge_t *) malloc(
                        sizeof(tree_edge_t) * initial_space);
    if (avl_tree->head != NULL) {
        _aux_raw_extractor(avl_tree->head, avl_raw, initial_space);
    }

    return avl_raw;
}

ADDCALL void ADDAPI avl_raw_destroy(tree_raw avl_raw_tree) {
    // TODO
}
