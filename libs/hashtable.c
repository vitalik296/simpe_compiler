#include "hashtable.h"

hashtable_t* hashtable_init(int capacity, float load_factor) {
    hashtable_t* hashtable = malloc(sizeof(hashtable_t));

    hashtable->size = 0;
    hashtable->capacity = (capacity > 0) ? capacity : DEFAULT_CAPACITY;
    hashtable->load_factor = (load_factor > 0) ? load_factor : DEFAULT_LOAD_FACTOR;
    hashtable->table = malloc(hashtable->capacity * sizeof(list_t*));

    for (int i = 0; i < hashtable->capacity; i++) {
        hashtable->table[i] = list_init();
    }

    return hashtable;
}

void hashtable_set(hashtable_t* hashtable, void* key, size_t key_size, int value) {
    uint16_t hash_num = hash_function(key, key_size);
    unsigned int index = (unsigned int) (hash_num % hashtable->capacity);

    hashtable_node_t* new_node = malloc(sizeof(hashtable_node_t));
    new_node->key = malloc(key_size);
    memcpy(new_node->key, key, key_size);
    new_node->key_size = key_size;
    new_node->value = value;

    list_t* list_to_insert = hashtable->table[index];

    if (list_to_insert->head != NULL) {
        list_node_t* current_node = list_to_insert->head;
        for (int i = 0; i < list_to_insert->size; i++) {
            if (!memcmp(((hashtable_node_t*) current_node->data)->key, key, key_size)) {
                ((hashtable_node_t*) current_node->data)->value = value;
                return;
            }
            current_node = current_node->next_node;
        }
    }

    list_insert(hashtable->table[index], new_node, sizeof(hashtable_node_t), 0);

    hashtable->size++;

    if ((float) hashtable->size / (float) hashtable->capacity > hashtable->load_factor) {
       hashtable_t* new_hashtable = hashtable_rehash(hashtable);
       hashtable->size = new_hashtable->size;
       hashtable->capacity = new_hashtable->capacity;
       hashtable->load_factor = new_hashtable->load_factor;
       hashtable->table = new_hashtable->table;
    }
}

hashtable_node_t* hashtable_get(hashtable_t* hashtable, void* key, size_t key_size) {
    uint16_t hash_value = hash_function(key, key_size);
    int index = (hash_value % hashtable->capacity);

    list_t* list_to_search = hashtable->table[index];
    list_node_t* getting_node = list_to_search->head;

    if (getting_node == NULL) {
        return NULL;
    }

    while (memcmp(((hashtable_node_t*) getting_node->data)->key, key, key_size) != 0) {
        getting_node = getting_node->next_node;

        if (getting_node == NULL) {
            return NULL;
        }
    }

    return getting_node->data;
}

hashtable_node_t* hashtable_remove(hashtable_t* hashtable, void* key, size_t key_size) {
    uint16_t hash_value = hash_function(key, key_size);
    int index = hash_value % hashtable->capacity;

    list_t* list_to_search = hashtable->table[index];
    list_node_t* getting_node = list_to_search->head;

    if (getting_node == NULL) {
        return NULL;
    }

    int i = 0;

    while (memcmp(((hashtable_node_t*) getting_node->data)->key, key, key_size) != 0) {
        getting_node = getting_node->next_node;

        if (getting_node == NULL) {
            return NULL;
        }

        i++;
    }

    list_delete(list_to_search, i);

    hashtable->size--;

    return getting_node->data;
}

hashtable_t* hashtable_rehash(hashtable_t* hashtable) {
    hashtable_t* new_hash = hashtable_init(hashtable->capacity*2, hashtable->load_factor);
    for( int i = 0; i < hashtable->capacity; i++)
    {
        if (hashtable->table[i]->size != 0)
        {
            list_node_t* current_list_node = hashtable->table[i]->head;
            while(current_list_node->next_node != NULL){
                hashtable_set(new_hash, ((hashtable_node_t*)(current_list_node->data))->key, ((hashtable_node_t*)(current_list_node->data))->key_size, ((hashtable_node_t*)(current_list_node->data))->value);
                current_list_node = current_list_node->next_node;
            }
            hashtable_set(new_hash, ((hashtable_node_t*)(current_list_node->data))->key, ((hashtable_node_t*)(current_list_node->data))->key_size, ((hashtable_node_t*)(current_list_node->data))->value);
        }
    }
    return new_hash;
}

void hashtable_free(hashtable_t* hashtable) {
    for (int i = 0; i < hashtable->capacity; i++) {
        list_free(hashtable->table[i]);
    }

    free(hashtable->table);
    free(hashtable);
}

void hashtable_add_str(hashtable_t* hash, char* key, int value){
    hashtable_set(hash, key, strlen(key), value);
}

void hashtable_add_int(hashtable_t* hash, int key, int value){
    hashtable_set(hash, &key, sizeof(int), value);
}

hashtable_node_t* hashtable_get_str(hashtable_t* hash, char* key){
    return hashtable_get(hash, key, strlen(key));
}

hashtable_node_t* hashtable_remove_str(hashtable_t* hash, char* key){
    return hashtable_remove(hash, key, strlen(key));
}

hashtable_node_t* hashtable_get_int(hashtable_t* hash, int key){
    return hashtable_get(hash, &key, sizeof(int));
}