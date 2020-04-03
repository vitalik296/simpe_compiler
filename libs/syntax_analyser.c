#include "syntax_analyser.h"

void traverse(tree_node_t* root, int deep){
    char* indent = memset(calloc(sizeof(char), deep * 2 + 1), '-', deep * 2);
    if (((not_terminal_node_t*)(root->data))->type == NOT_TERMINAL_NODE){
        printf("%s<%s>\n", indent, ((not_terminal_node_t*)(root->data))->text);
    }
    else{
        printf("%s%d %s\n", indent, ((terminal_node_t*)(root->data))->code,  ((terminal_node_t*)(root->data))->text);
    }
    free(indent);
    deep++;
    for(int i = 0; i < root->children_count; i++){
        traverse(root->children[i], deep);
    }
}

terminal_node_t* terminal_init(int code, char* text){
    terminal_node_t* terminal_node = malloc(sizeof(terminal_node_t));
    terminal_node->type = TERMINAL_NODE;
    terminal_node->code = code;
    terminal_node->text = calloc(sizeof(char), strlen(text) + 1);
    strcpy(terminal_node->text, text);
    return terminal_node;
}

void terminal_free(terminal_node_t* node){
    free(node->text);
    free(node);
}


not_terminal_node_t* not_terminal_init(char* text){
    not_terminal_node_t* not_terminal_node = malloc(sizeof(not_terminal_node_t));
    not_terminal_node->type = NOT_TERMINAL_NODE;
    not_terminal_node->text = calloc(sizeof(char), strlen(text) + 1);
    strcpy(not_terminal_node->text, text);
    return  not_terminal_node;
}

void not_terminal_free(not_terminal_node_t* node){
    free(node->text);
    free(node);
}


syntax_analyser_t* syntax_init(list_t* tokens){
    syntax_analyser_t* syntax_analyser = malloc(sizeof(syntax_analyser_t));

    syntax_analyser->tokens = tokens;
    syntax_analyser->current_node = NULL;
    //    syntax_analyser->current_node_pos = 0;

    return syntax_analyser;
}

void syntax_free(syntax_analyser_t* syntax_analyser){
    list_free(syntax_analyser->tokens);
    free(syntax_analyser);
}




void err(lexem_t* lexeme, char* expected, tree_node_t* curr_node){
    while (curr_node->parent != NULL)
        curr_node = curr_node->parent;

    traverse(curr_node, 0);
    printf("Invalid token (%d %d): %s found, but %s expected.\n", lexeme->row, lexeme->col, lexeme->text, expected);

    exit(1);
}


list_node_t* get_token(syntax_analyser_t* syntax_analyser){
    if (syntax_analyser->current_node != NULL) {
        syntax_analyser->current_node = syntax_analyser->current_node->next_node;
    }else{
        syntax_analyser->current_node = syntax_analyser->tokens->head;
    }

    return syntax_analyser->current_node;
}


bool is_identifier(lexem_t* lexeme){
    if (lexeme->code > 1000)
        return true;
    return false;
}

void add_terminal_node(tree_node_t* node, lexem_t* curr_lexeme, int required_code, char* text_expected){
    if ((required_code != -1) && (curr_lexeme->code != required_code)){
        err(curr_lexeme, text_expected, node);
    }

    terminal_node_t* new_term_node = terminal_init(curr_lexeme->code, curr_lexeme->text);
    tree_insert(node, tree_node_init(new_term_node, sizeof(terminal_node_t)));

}

tree_node_t* add_not_terminal_node(char* text, tree_node_t* node){
    tree_node_t* new_node = tree_node_init(not_terminal_init(text), sizeof(not_terminal_node_t));
    tree_insert(node, new_node);
    return new_node;
}




void identifier(tree_node_t* node, syntax_analyser_t* syntax_analyser, bool token){
    tree_node_t* new_node = add_not_terminal_node("identifier", node);

    list_node_t* curr_node;

    if (token == true) {
        curr_node = get_token(syntax_analyser);
    }
    else {
        curr_node = syntax_analyser->current_node;
    }

    if(is_identifier((lexem_t*)(curr_node->data))){
        add_terminal_node(new_node, (lexem_t*)(curr_node->data), -1, "");
    }else{
        err(((lexem_t*)(curr_node->data)), "identifier", new_node);
    }

}

void procedure_identifier(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("procedure_identifier", node);
    identifier(new_node, syntax_analyser, true);
}

void unsigned_integer(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("unsigned_integer", node);

    int lex_code = ((lexem_t*)(syntax_analyser->current_node->data))->code;

    if( lex_code > 500 && lex_code < 1000){
        add_terminal_node(new_node, (lexem_t*)(syntax_analyser->current_node->data), -1, "");
    } else {
        err(((lexem_t*)(syntax_analyser->current_node->data)), "constant(number)", new_node);
    }
}

void right_part(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("right_part", node);

    list_node_t* curr_node = syntax_analyser->current_node;

    if (((lexem_t*)(curr_node->data))->code == 44) {
        add_terminal_node(new_node, (lexem_t*) (curr_node->data), 44, ",");

        get_token(syntax_analyser);
        unsigned_integer(new_node, syntax_analyser);
        get_token(syntax_analyser);

    } else if (((lexem_t*)(curr_node->data))->code == 405){
        add_terminal_node(new_node, (lexem_t*) (curr_node->data), 405, "$EXP");

        curr_node = get_token(syntax_analyser);
        add_terminal_node(new_node, (lexem_t*) (curr_node->data), 40, "(");

        get_token(syntax_analyser);
        unsigned_integer(new_node, syntax_analyser);


        curr_node = get_token(syntax_analyser);
        add_terminal_node(new_node, (lexem_t*) (curr_node->data), 41, ")");
        get_token(syntax_analyser);
    } else {
        add_not_terminal_node("empty", new_node);
    }
}

void left_part(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("left_part", node);

    list_node_t* curr_node = get_token(syntax_analyser);

    if (((lexem_t*)(curr_node->data))->code / 500 != 1) {
        add_not_terminal_node("empty", new_node);
    } else {
        unsigned_integer(new_node, syntax_analyser);
        get_token(syntax_analyser);
    }

}


void complex_number(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("complex_number", node);

    left_part(new_node, syntax_analyser);
    right_part(new_node, syntax_analyser);
}

void constant(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("constant", node);

    list_node_t* curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*) (curr_node->data), 39, "'");

    complex_number(new_node, syntax_analyser);

    add_terminal_node(new_node, (lexem_t*) (syntax_analyser->current_node->data), 39, "'");
}

void constant_identifier(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("constant_identifier", node);
    identifier(new_node, syntax_analyser, false);
}


void constant_declaration(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("constant_declaration", node);

    constant_identifier(new_node, syntax_analyser);

    list_node_t* curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*) (curr_node->data), 61, "=");

    constant(new_node, syntax_analyser);

    curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*) (curr_node->data), 59, ";");

}

//void constant_declaration_list(tree_node_t* node, syntax_analyser_t* syntax_analyser){
//    tree_node_t* new_node = add_not_terminal_node("constant_declarations_list", node);
//
//    list_node_t* curr_node = get_token(syntax_analyser);
//
//    if (((lexem_t*)(curr_node->data))->code == 402) {
//        add_not_terminal_node("empty", new_node);
//    } else {
//        while (((lexem_t*)(curr_node->data))->code != 402){
//            constant_declaration(new_node, syntax_analyser);
//            curr_node = get_token(syntax_analyser);
//        }
//    }
//}

void constant_declaration_list(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("constant_declarations_list", node);

    list_node_t* curr_node = get_token(syntax_analyser);

    if (((lexem_t*)(curr_node->data))->code == 402) {
        add_not_terminal_node("empty", new_node);
    } else {
        constant_declaration(new_node, syntax_analyser);
        constant_declaration_list(new_node, syntax_analyser);
    }
}


void constant_declarations(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("constant_declarations", node);

    list_node_t* curr_node = get_token(syntax_analyser);

    if (((lexem_t*)(curr_node->data))->code == 402) {
        add_not_terminal_node("empty", new_node);
    } else {
        add_terminal_node(new_node, (lexem_t*) (curr_node->data), 404, "CONST");
        constant_declaration_list(new_node, syntax_analyser);
    }


}

void declarations(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("declarations", node);

    constant_declarations(new_node, syntax_analyser);

}

void statements_list(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("statements_list", node);
    add_not_terminal_node("empty", new_node);

}

void block(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("block", node);

    declarations(new_node, syntax_analyser);

//    list_node_t* curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*)(syntax_analyser->current_node->data), 402, "BEGIN");

    statements_list(new_node, syntax_analyser);

    list_node_t* curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*)(curr_node->data), 403, "END");
}

void program(tree_node_t* node, syntax_analyser_t* syntax_analyser){
    tree_node_t* new_node = add_not_terminal_node("program", node);

    list_node_t* curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*)(curr_node->data), 401, "PROGRAM");


    procedure_identifier(new_node, syntax_analyser);

    curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*)(curr_node->data), 59, ";");

    block(new_node , syntax_analyser);

    curr_node = get_token(syntax_analyser);
    add_terminal_node(new_node, (lexem_t*)(curr_node->data), 46, ".");
}

void signal_program(tree_node_t* node, syntax_analyser_t* syntax_analyser){
//    tree_node_t* new_node = add_not_terminal_node("signal_program", node);
    program(node, syntax_analyser);
}


tree_node_t* syntax_parse(syntax_analyser_t* syntax_analyser){
    tree_node_t* root = tree_node_init(not_terminal_init("signal_program"), sizeof(not_terminal_node_t));
    signal_program(root, syntax_analyser);
    return root;
}
