#include "tree.h"

tree_node_t* tree_node_init(void* data, size_t data_size){
    tree_node_t* tree_node = malloc(sizeof(tree_node_t));

    tree_node->parent = NULL;
    tree_node->children = NULL;
    tree_node->children_count = 0;
    tree_node->data_size = data_size;

    tree_node->data = malloc(data_size);
    memcpy(tree_node->data, data, data_size);

    return tree_node;
}

void tree_node_free(tree_node_t* tree_node){
    tree_node->parent = NULL;
    tree_node->children = NULL;
    free(tree_node->data);
    free(tree_node);
}


void tree_insert(tree_node_t* root, tree_node_t* new_node){
    if (root != NULL) {
        root->children_count++;
        root->children = realloc(root->children, sizeof(void*) * root->children_count);
        root->children[root->children_count-1] = new_node;
        new_node->parent = root;
    }
}
