#ifndef COMPILATOR_HASHTABLE_H
#define COMPILATOR_HASHTABLE_H

#include "common.h"
#include "list.h"

#define DEFAULT_CAPACITY 150
#define DEFAULT_LOAD_FACTOR 2.0f

typedef struct {
    void* key;
    int key_size;
    int value;
} hashtable_node_t;

typedef struct {
    list_t** table;
    int size;
    int capacity;
    float load_factor;
} hashtable_t;

hashtable_t* hashtable_init(int capacity, float load_factor);
void hashtable_set(hashtable_t* hashtable, void* key, size_t key_size, int value);
hashtable_node_t* hashtable_get(hashtable_t* hashtable, void* key, size_t key_size);
hashtable_node_t* hashtable_remove(hashtable_t* hashtable, void* key, size_t key_size);
hashtable_t* hashtable_rehash(hashtable_t* hashtable);
void hashtable_add_str(hashtable_t* hash, char* key, int value);
void hashtable_add_int(hashtable_t* hash, int key, int value);
hashtable_node_t* hashtable_get_str(hashtable_t* hash, char* key);
hashtable_node_t* hashtable_remove_str(hashtable_t* hash, char* key);
hashtable_node_t* hashtable_get_int(hashtable_t* hash, int key);
void hashtable_free(hashtable_t* hashtable);

#endif //UTILS_HASHTABLE_H