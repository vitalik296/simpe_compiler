#include <unistd.h>
#include <fcntl.h>

#include "lexem_analyser.h"


void lexem_print(lexem_t* lexem){
    printf("%d\t%d\t%d\t%s\n", lexem->row, lexem->col, lexem->code, lexem->text);
}

lexem_t* lexem_init(int row, int col, char* text, int code){
    lexem_t* new_lexem = malloc(sizeof(lexem_t));
    new_lexem->row = row;
    new_lexem->col = col;
    new_lexem->code = code;
    new_lexem->text = malloc(strlen(text) + 1);
    strcpy(new_lexem->text, text);
    return new_lexem;
}

void lexem_free(lexem_t* lexem){
    free(lexem->text);
    free(lexem);
}

void error(char c, int row, int col, char* err) {
    printf("Lexer: Error (line %d, column %d): %s '%c' (code: %d)\n", row, col, err, c, c);
}


void add_keywords(hashtable_t* hash){
    hashtable_add_str(hash, "PROGRAM", 401);
    hashtable_add_str(hash, "BEGIN", 402);
    hashtable_add_str(hash, "END", 403);
    hashtable_add_str(hash, "CONST", 404);
    hashtable_add_str(hash, "$EXP", 405);
}

void add_delimiters(hashtable_t* hash){
    hashtable_add_str(hash, ";", 59);
    hashtable_add_str(hash, "'", 39);
    hashtable_add_str(hash, ",", 44);
    hashtable_add_str(hash, "(", 40);
    hashtable_add_str(hash, ")", 41);
    hashtable_add_str(hash, "=", 61);
    hashtable_add_str(hash, ".", 46);
}

char* get_next_symbol(analyser_t* analyser){
    if(analyser->enter){
        analyser->row++;
        analyser->col = 0;
        analyser->enter=false;
    }
    if (analyser->file_descriptor == -1)
        return NULL;
    char* buff = calloc(2,sizeof(char));
    for(int i =0; i< 2; i++)
        buff[i] = '\0';
    int check = -1;
    check = read(analyser->file_descriptor, buff,1);
    if (check < 0)
        return NULL;
    else {
        char symb = buff[0];
        if (symb == '\n'){
            analyser->enter = true;
            //return get_next_symbol(analyser);
            return buff;
        } else if(symb == '\0'){
            return NULL;
        } else{
            analyser->col++;
        }
        return buff;
    }
}

bool is_letter(char chr){
    return chr >= 'A' && chr <= 'Z';
}

bool is_number(char chr){
    return chr >= '0' && chr <= '9';
}

bool is_whitespace(char chr){
    switch(chr){
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 32:
            return true;
        default:
            return false;
    }
}

bool is_delimiters(char chr){
    switch(chr){
        case ';':
        case '\'':
        case ',':
        case '=':
        case ')':
        case '.':
            return true;
        default:
            return false;
    }
}

analyser_t* analyser_init(char* input_file){
    analyser_t* analyser = malloc(sizeof(analyser_t));

    analyser->keywords = hashtable_init(15, 0.7f);
    add_keywords(analyser->keywords);
    analyser->delimiters = hashtable_init(15, 0.7f);
    add_delimiters(analyser->delimiters);
    analyser->constants = hashtable_init(100, 0.7f);
    analyser->identifier = hashtable_init(100, 0.7f);

    analyser->col = 0;
    analyser->row = 1;
    analyser->ident_num = 0;
    analyser->const_num = 0;
    analyser->buff = calloc(256,sizeof(char));
    analyser->buff_len = 0;
    analyser->max_capacity= 256;
    analyser->file_descriptor = open(input_file, O_RDONLY);
    analyser->enter = false;

    return analyser;
}

void analyser_free(analyser_t* analyser){
    free(analyser->buff);
    close(analyser->file_descriptor);

    hashtable_free(analyser->identifier);
    hashtable_free(analyser->keywords);
    hashtable_free(analyser->constants);
    hashtable_free(analyser->delimiters);

    free(analyser);
}

int analyser_lexem_identify(analyser_t* analyser){

    char* text = malloc(analyser->buff_len);
    strncat(text, analyser->buff, analyser->buff_len);

    hashtable_node_t* node = hashtable_get_str(analyser->keywords, text);
    if (node != NULL){
        return node->value;
    }

    node = hashtable_get_str(analyser->identifier, text);
    if (node != NULL){
        return node->value;
    }

    node = hashtable_get_str(analyser->delimiters, text);
    if (node != NULL){
        return node->value;
    }

    node = hashtable_get_str(analyser->constants, text);
    if (node != NULL){
        return node->value;
    }

    if (!strcmp(analyser->buff, "0") || atoi(analyser->buff)){
        int code = 501 + analyser->const_num;

        hashtable_add_str(analyser->constants, text, code);
        analyser->const_num++;
        return code;
    }

    int code = 1001 + analyser->ident_num;

    hashtable_add_str(analyser->identifier, text, code);
    analyser->ident_num++;

    return code;
}

void add_to_buff(analyser_t* analyser, char* symb){
    if (analyser->buff_len + 1 >= analyser->max_capacity){
        analyser->max_capacity *= 2;
        analyser->buff = realloc(analyser->buff, analyser->max_capacity);
    }
    strncat(analyser->buff, symb, 1);
    analyser->buff_len++;
}

void clean_buffer(analyser_t* analyser){
    analyser->buff_len = 0;
    analyser->buff[0] = '\0';
}

void create_lexem(analyser_t* analyser, list_t* lexemes, bool sub){
    int col = analyser->col;
    if (sub){
        col -= analyser->buff_len;
    }

    list_insert(lexemes, lexem_init(analyser->row, col, analyser->buff, analyser_lexem_identify(analyser)), sizeof(lexem_t), -1);
    clean_buffer(analyser);
}

list_t* analyser_parse(analyser_t* analyser){
    list_t* lexemes = list_init();
    bool is_comment = false;
    char* symb = get_next_symbol(analyser);;

    while(true){

        if (symb == NULL)
            break;

        if (is_letter(*symb)){
            do {
                add_to_buff(analyser, symb);
                symb = get_next_symbol(analyser);
                if (symb == NULL)
                    break;
            }while (is_letter(*symb) || is_number(*symb));

            create_lexem(analyser, lexemes, true);
            if (symb == NULL)
                break;
            continue;
        }

        if (*symb == '$'){
            add_to_buff(analyser, symb);
            symb = get_next_symbol(analyser);
            if (*symb == 'E'){
                add_to_buff(analyser, symb);
                symb = get_next_symbol(analyser);
                if (*symb == 'X'){
                    add_to_buff(analyser, symb);
                    symb = get_next_symbol(analyser);
                    if (*symb == 'P'){
                        add_to_buff(analyser, symb);
                        symb = get_next_symbol(analyser);

                        if (symb == NULL){
                            error('$', analyser->row, analyser->col - analyser->buff_len, "Illegal symbol");
                            return lexemes;
                        }

                        if (*symb != '(') {
                            error('$', analyser->row, analyser->col - analyser->buff_len, "Illegal symbol");
                            return lexemes;
                        } else {
                            create_lexem(analyser, lexemes, true);
                            add_to_buff(analyser, symb);
                            create_lexem(analyser, lexemes, false);
                            symb = get_next_symbol(analyser);
                            continue;
                        }
                    }
                    else{
                        error('$', analyser->row, analyser->col - analyser->buff_len, "Illegal symbol");
                        return lexemes;
                    }
                }
                else{
                    error('$', analyser->row, analyser->col - analyser->buff_len, "Illegal symbol");
                    return lexemes;
                }
            }
            else{
                error('$', analyser->row, analyser->col - analyser->buff_len, "Illegal symbol");
                return lexemes;
            }
        }

        if (is_number(*symb)){
            do {
                add_to_buff(analyser, symb);
                symb = get_next_symbol(analyser);
                if (symb == NULL)
                    break;
            }while (is_number(*symb));

            if (symb != NULL && is_letter(*symb)){
                error(analyser->buff[0], analyser->row, analyser->col - analyser->buff_len - 1, "Illegal symbol");
                return lexemes;
            }

            create_lexem(analyser, lexemes, true);
            if (symb == NULL)
                break;
            continue;
        }

        if (is_whitespace(*symb)){
            symb = get_next_symbol(analyser);
            continue;
        }

        if (is_delimiters(*symb)){
            add_to_buff(analyser, symb);
            create_lexem(analyser, lexemes, false);
            symb = get_next_symbol(analyser);
            continue;
        }

        if (*symb == '('){
            char tmp = *symb;
            symb = get_next_symbol(analyser);

            if (symb == NULL){
                create_lexem(analyser, lexemes, true);
                break;
            }

            if (*symb != '*'){
                add_to_buff(analyser, &tmp);
                create_lexem(analyser, lexemes, true);
                continue;
            }
            else{
                while (true){
                    symb = get_next_symbol(analyser);

                    if (symb == NULL){
                        error('\0', analyser->row, analyser->col, "Comment error: eof");
                        return lexemes;
                    }

                    if (*symb == '*'){
                        symb = get_next_symbol(analyser);

                        if (symb == NULL){
                            error('\0', analyser->row, analyser->col, "Comment error: eof");
                            return lexemes;
                        }

                        if (*symb != ')'){
                            continue;
                        } else {
                            symb = get_next_symbol(analyser);
                            break;
                        }
                    } else continue;
                }
                continue;
            }

        }

        error(*symb, analyser->row, analyser->col - 1, "Illegal symbol");
        return lexemes;
    }
    return lexemes;
}

