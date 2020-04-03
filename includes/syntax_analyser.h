#ifndef COMPILATOR_SYNTAX_ANALYSER_H
#define COMPILATOR_SYNTAX_ANALYSER_H

#include "list.h"
#include "hashtable.h"
#include "tree.h"
#include "lexem_analyser.h"

#define TERMINAL_NODE 0
#define NOT_TERMINAL_NODE 1

typedef struct{
    short type;
    int code;
    char* text;
}terminal_node_t;

typedef struct{
    short type;
    char* text;
}not_terminal_node_t;

typedef struct{
    list_t* tokens;
    list_node_t* current_node;
}syntax_analyser_t;

void traverse(tree_node_t* root, int deep);

terminal_node_t* terminal_init(int code, char* text);
void terminal_free(terminal_node_t* node);

not_terminal_node_t* not_terminal_init(char* text);
void not_terminal_free(not_terminal_node_t* node);

syntax_analyser_t* syntax_init(list_t* tokens);
void syntax_free(syntax_analyser_t* syntax_analyser);

tree_node_t* syntax_parse(syntax_analyser_t* syntax_analyser);

#endif //COMPILATOR_SYNTAX_ANALYSER_H
