#ifndef COMPILATOR_TREE_H
#define COMPILATOR_TREE_H

#include "common.h"

typedef struct tree_node_s{
    struct tree_node_s* parent;
    struct tree_node_s** children;
    size_t children_count;
    void* data;
    size_t data_size;
} tree_node_t;


tree_node_t* tree_node_init(void* data, size_t data_size);
void tree_node_free(tree_node_t* tree_node);


void tree_insert(tree_node_t* parent, tree_node_t* new_node);

#endif //COMPILATOR_TREE_H
