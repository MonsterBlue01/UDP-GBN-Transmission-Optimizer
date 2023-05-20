#include "kvs_lru.h"

#include <stdlib.h>

typedef struct lru_node {
    char* key;
    struct lru_node* prev;
    struct lru_node* next;
} lru_node;

struct kvs_lru {
    kvs_base_t* kvs_base;
    int capacity;
    int size;
    lru_node* head;
    lru_node* tail;
    lru_node** nodes;
};

kvs_lru_t* kvs_lru_new(kvs_base_t* kvs, int capacity) {
    kvs_lru_t* kvs_lru = malloc(sizeof(kvs_lru_t));
    kvs_lru->kvs_base = kvs;
    kvs_lru->capacity = capacity;
    kvs_lru->size = 0;
    kvs_lru->head = NULL;
    kvs_lru->tail = NULL;
    kvs_lru->nodes = malloc(capacity * sizeof(lru_node*));
    for (int i = 0; i < capacity; i++) {
        kvs_lru->nodes[i] = NULL;
    }
    return kvs_lru;
}

void kvs_lru_free(kvs_lru_t** ptr) {
    lru_node* current = (*ptr)->head;
    while (current != NULL) {
        lru_node* next = current->next;
        free(current->key);
        free(current);
        current = next;
    }
    free((*ptr)->nodes);
    free(*ptr);
    *ptr = NULL;
}

static void move_to_front(kvs_lru_t* kvs_lru, lru_node* node) {
    if (node == kvs_lru->head) {
        return;
    }
    if (node == kvs_lru->tail) {
        kvs_lru->tail = node->prev;
    }
    node->prev->next = node->next;
    if (node->next != NULL) {
        node->next->prev = node->prev;
    }
    node->prev = NULL;
    node->next = kvs_lru->head;
    kvs_lru->head->prev = node;
    kvs_lru->head = node;
}

int kvs_lru_set(kvs_lru_t* kvs_lru, const char* key, const char* value) {
    // Set the value in the underlying storage
    int result = kvs_base_set(kvs_lru->kvs_base, key, value);
    if (result != 0) {
        return result;
    }
    
    // Check if the key is already in the cache
    for (int i = 0; i < kvs_lru->size; i++) {
        if (strcmp(kvs_lru->nodes[i]->key, key) == 0) {
            // The key is in the cache, move it to the front
            move_to_front(kvs_lru, kvs_lru->nodes[i]);
            return 0;
        }
    }
    
    // The key is not in the cache, add it
    lru_node* node = malloc(sizeof(lru_node));
    node->key = strdup(key);
    node->prev = NULL;
    node->next = kvs_lru->head;
    if (kvs_lru->head != NULL) {
        kvs_lru->head->prev = node;
    }
    kvs_lru->head = node;
    if (kvs_lru->tail == NULL) {
        kvs_lru->tail = node;
    }
    kvs_lru->nodes[kvs_lru->size++] = node;
    
    // Check if the cache is full
    if (kvs_lru->size > kvs_lru->capacity) {
        // Cache is full, need to evict the least recently used key
        lru_node* tail = kvs_lru->tail;
        kvs_lru->tail = tail->prev;
        kvs_lru->tail->next = NULL;
        for (int i = 0; i < kvs_lru->size; i++) {
            if (kvs_lru->nodes[i] == tail) {
                kvs_lru->nodes[i] = kvs_lru->nodes[--kvs_lru->size];
                break;
            }
        }
        free(tail->key);
        free(tail);
    }
    
    return 0;
}

int kvs_lru_get(kvs_lru_t* kvs_lru, const char* key, char* value) {
    // Check if the key is in the cache
    for (int i = 0; i < kvs_lru->size; i++) {
        if (strcmp(kvs_lru->nodes[i]->key, key) == 0) {
            // The key is in the cache, move it to the front
            move_to_front(kvs_lru, kvs_lru->nodes[i]);
            // Get the value from the underlying storage
            return kvs_base_get(kvs_lru->kvs_base, key, value);
        }
    }
    
    // The key is not in the cache, get the value from the underlying storage
    return kvs_base_get(kvs_lru->kvs_base, key, value);
}

int kvs_lru_flush(kvs_lru_t* kvs_lru) {
    // Clear the cache
    lru_node* current = kvs_lru->head;
    while (current != NULL) {
        lru_node* next = current->next;
        free(current->key);
        free(current);
        current = next;
    }
    kvs_lru->head = NULL;
    kvs_lru->tail = NULL;
    kvs_lru->size = 0;
    return 0;
}
