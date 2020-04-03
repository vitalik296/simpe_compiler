#include <stdio.h>

#include "hashtable.h"
#include "lexem_analyser.h"
#include "syntax_analyser.h"
#include "translator.h"

void print_hashtable(hashtable_t* hash){
    int table_index = 0;
    list_t* current_list = hash->table[table_index];

    for(int i = 0; i < hash->size;){
        if(current_list->size != 0){
            list_node_t * current_node = current_list->head;
            while (current_node->next_node != NULL){
                printf("%s\t%d\n", (char*)((hashtable_node_t*)current_node->data)->key, ((hashtable_node_t*)current_node->data)->value);
                current_node = current_node->next_node;
                i++;
            }
            printf("%s\t%d\n", (char*)((hashtable_node_t*)current_node->data)->key, ((hashtable_node_t*)current_node->data)->value);
            i++;
            table_index++;
            current_list = hash->table[table_index];
        }
        else {
            table_index++;
            current_list = hash->table[table_index];
        }
    }
}


int main(int argc,char** argv) {
    if(argc != 2){
        puts("Please enter file name");
        return 1;
    }

    char* input_file = argv[1];

    analyser_t* analyser = analyser_init(input_file);

    list_t* lexemes = analyser_parse(analyser);

    if (lexemes == NULL)
        return 1;

    puts("Constant:");
    print_hashtable(analyser->constants);

    puts("");
    puts("Identifiers:");
    print_hashtable(analyser->identifier);

    puts("");
    puts("Tokens:");

    list_node_t* current_node = lexemes->head;
    for (size_t i = 0; i < lexemes->size; i++){
        lexem_t* lexem = current_node->data;
        lexem_print(lexem);
        current_node = current_node->next_node;
    }

    syntax_analyser_t* syntax_analyser = syntax_init(lexemes);
    tree_node_t* root = syntax_parse(syntax_analyser);

    traverse(root, 0);


    translator_t* translator = translator_init(analyser->identifier, analyser->constants);

    translate(root, translator);

    printf("\nGenerated Code:\n\n");

    puts(translator->text);

//    analyser_free(analyser);
//    syntax_free(syntax_analyser);
    return 0;
}