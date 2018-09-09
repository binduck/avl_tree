#ifndef _AVL_H
#define _AVL_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct tree *tree_t;

typedef struct tree_edge {
    unsigned long v, u;
} tree_edge_t;

typedef struct tree_raw {
    unsigned long count_edges;
    tree_edge_t *edges;
} *tree_raw;

#ifdef _WIN32
    #ifdef ADD_EXPORTS
        #define ADDAPI __declspec(dllexport)
    #else
        #define ADDAPI __declspec(dllimport)
    #endif

    #define ADDCALL __cdecl
#else
    #define ADDAPI
    #define ADDCALL
#endif

#ifdef __cplusplus
extern "C"
{
#endif

ADDCALL tree_t ADDAPI avl_create();
ADDCALL void ADDAPI avl_add(tree_t avl_tree, long value);
ADDCALL bool ADDAPI avl_exists(tree_t avl_tree, long value);
ADDCALL void ADDAPI avl_destroy(tree_t avl_tree);

ADDCALL tree_raw ADDAPI avl_get_raw_data(tree_t avl_tree);
ADDCALL void ADDAPI avl_raw_destroy(tree_raw avl_raw_tree);

#ifdef __cplusplus
}
#endif

#endif
