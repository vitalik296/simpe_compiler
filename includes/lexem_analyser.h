#ifndef COMPILATOR_LEXEM_ANALYSER_H
#define COMPILATOR_LEXEM_ANALYSER_H

#include "common.h"
#include "hashtable.h"

typedef struct {
    int row;
    int col;
    int code;
    char* text;
}lexem_t;

typedef struct{
    hashtable_t* keywords;
    hashtable_t* delimiters;
    hashtable_t* constants;
    hashtable_t* identifier;
    int file_descriptor;
    char* buff;
    int buff_len;
    int max_capacity;
    int row;
    int col;
    int ident_num;
    int const_num;
    bool enter;
}analyser_t;

void lexem_print(lexem_t* lexem);
lexem_t* lexem_init(int row, int col, char* text, int code);
void lexem_free(lexem_t* lexem);

analyser_t* analyser_init(char* input_file);
list_t* analyser_parse(analyser_t* analyser);
void analyser_free(analyser_t* analyser);

#endif //COMPILATOR_LEXEM_ANALYSER_H
