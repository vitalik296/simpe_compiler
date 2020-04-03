#ifndef COMPILATOR_TRANSLATOR_H
#define COMPILATOR_TRANSLATOR_H

#include "common.h"
#include "tree.h"
#include "lexem_analyser.h"
#include "syntax_analyser.h"

typedef struct{
    char* text;
    char* program_name;
    hashtable_t* identifiers;
    hashtable_t* constant;
}translator_t;

void translate(tree_node_t* root, translator_t* translator);
translator_t* translator_init(hashtable_t* identifiers, hashtable_t* constant);
void translator_free(translator_t* translator);

#endif //COMPILATOR_TRANSLATOR_H
