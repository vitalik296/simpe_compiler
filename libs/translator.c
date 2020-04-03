#include "translator.h"


translator_t* translator_init(hashtable_t* identifiers, hashtable_t* constant){
    translator_t* translator = malloc(sizeof(translator_t));
    translator->text = calloc(1,1);
    translator->program_name = calloc(1,1);
    translator->constant = constant;
    translator->identifiers = identifiers;
    return translator;
}

void translator_free(translator_t* translator){
    free(translator->text);
    free(translator->program_name);
    hashtable_free(translator->identifiers);
    hashtable_free(translator->constant);
    free(translator);
}


void exception( char* identifier){
    printf("Not unique name: %s", identifier);

    exit(1);
}

char* strconcat(char* to, char* from){
    to = realloc(to, strlen(to) + strlen(from) + 1);
    strcat(to, from);
    return to;
}

char* tr_identifier(tree_node_t* node, translator_t* translator){
    tree_node_t* identifier = node->children[0];
    hashtable_node_t* ident = hashtable_remove_str(translator->identifiers, ((terminal_node_t*)(identifier->data))->text);

    if (ident == NULL)
        exception(((terminal_node_t*)(identifier->data))->text);

    translator->text = strconcat(translator->text, ((terminal_node_t*)(identifier->data))->text);

    return ((terminal_node_t*)(identifier->data))->text;

}

void tr_procedure_identifier(tree_node_t* node, translator_t* translator){
    tr_identifier(node->children[0], translator);

    translator->program_name = calloc(1, strlen(translator->text) + 1);
    strcpy(translator->program_name, translator->text);

}

void tr_unsigned_integer(tree_node_t* node, translator_t* translator){
    tree_node_t* constant = node->children[0];
    hashtable_node_t* ident = hashtable_get_str(translator->constant, ((terminal_node_t*)(constant->data))->text);

    if (ident == NULL)
        exception(((terminal_node_t*)(constant->data))->text);

    translator->text = strconcat(translator->text, ((terminal_node_t*)(constant->data))->text);
}

void tr_right_part(tree_node_t* node, translator_t* translator){
    if (!strcmp(((not_terminal_node_t*)node->children[0]->data)->text, "empty"))
        return;

    for (int i = 0; i < node->children_count; i++){
        tree_node_t* tmp_node = node->children[i];
        if (((not_terminal_node_t*)tmp_node->data)->type == TERMINAL_NODE)
            translator->text = strconcat(translator->text, ((terminal_node_t*)tmp_node->data)->text);
        else{
            tr_unsigned_integer(tmp_node, translator);
        }
    }
}

void tr_left_part(tree_node_t* node, translator_t* translator){
    if (!strcmp(((not_terminal_node_t*)node->children[0]->data)->text, "empty"))
        return;

    tr_unsigned_integer(node->children[0], translator);
}

void tr_complex_number(tree_node_t* node, translator_t* translator){
    tr_left_part(node->children[0], translator);
    tr_right_part(node->children[1], translator);
}

void tr_constant(tree_node_t* node, translator_t* translator){
    translator->text = strconcat(translator->text, "\"");
    tr_complex_number(node->children[1], translator);
    translator->text = strconcat(translator->text, "\"");
}

char* tr_constant_identifier(tree_node_t* node, translator_t* translator){
    translator->text = strconcat(translator->text, "\t");
    return tr_identifier(node->children[0], translator);
}

void tr_constant_declaration(tree_node_t* node, translator_t* translator){
    char* const_id = tr_constant_identifier(node->children[0], translator);

    translator->text = strconcat(translator->text, " db ");

    tr_constant(node->children[2], translator);

    translator->text = strconcat(translator->text, "\n");

    translator->text = strconcat(translator->text, "\t_len_");
    translator->text = strconcat(translator->text, const_id);

    translator->text = strconcat(translator->text, " equ $ - ");
    translator->text = strconcat(translator->text, const_id);

}


void tr_constant_declaration_list(tree_node_t* node, translator_t* translator){
    if (node->children_count <= 1)
        return;

    tr_constant_declaration(node->children[0], translator);
    translator->text = strconcat(translator->text, "\n");
    tr_constant_declaration_list(node->children[1], translator);
}


void tr_constant_declarations(tree_node_t* node, translator_t* translator){
    if (node->children_count <= 1)
        return;

    translator->text = strconcat(translator->text, "section .data\n");

    tr_constant_declaration_list(node->children[1], translator);

    translator->text = strconcat(translator->text, "\n");
}

void tr_declarations(tree_node_t* node, translator_t* translator){
    tr_constant_declarations(node->children[0], translator);
}


void tr_block(tree_node_t* node, translator_t* translator){
    tr_declarations(node->children[0], translator);

    translator->text = strconcat(translator->text, "section .text\n");
    translator->text = strconcat(translator->text, "\tglobal ");
    translator->text = strconcat(translator->text, translator->program_name);
}

void tr_program(tree_node_t* node, translator_t* translator){
    tree_node_t* tmp_node = node->children[1];

    tr_procedure_identifier(node->children[1], translator);
    translator->text = strconcat(translator->text, ":\n");
    translator->text = strconcat(translator->text, "\tmov eax, 1\t; system call number (sys_exit)\n");
    translator->text = strconcat(translator->text, "\tint 0x80\t; call kernel\n");
    translator->text = strconcat(translator->text, "\n");

    tr_block(node->children[3], translator);

}

void tr_signal_program(tree_node_t* node, translator_t* translator){
    tr_program(node->children[0], translator);
}


void translate(tree_node_t* root, translator_t* translator){
    tr_signal_program(root, translator);
}