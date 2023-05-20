#include "kvs_fifo.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

struct kvs_fifo {
    kvs_base_t* kvs_base;
    int capacity;
    int size;
    int front;
    int back;
    char** keys;
};

kvs_fifo_t* kvs_fifo_new(kvs_base_t* kvs, int capacity) {
    kvs_fifo_t* kvs_fifo = malloc(sizeof(kvs_fifo_t));
    kvs_fifo->kvs_base = kvs;
    kvs_fifo->capacity = capacity;
    kvs_fifo->size = 0;
    kvs_fifo->front = 0;
    kvs_fifo->back = 0;
    kvs_fifo->keys = malloc(capacity * sizeof(char*));
    for (int i = 0; i < capacity; i++) {
        kvs_fifo->keys[i] = NULL;
    }
    return kvs_fifo;
}

void kvs_fifo_free(kvs_fifo_t** ptr) {
    for (int i = 0; i < (*ptr)->capacity; i++) {
        free((*ptr)->keys[i]);
    }
    free((*ptr)->keys);
    free(*ptr);
    *ptr = NULL;
}

int kvs_fifo_set(kvs_fifo_t* kvs_fifo, const char* key, const char* value) {
    // Set the value in the underlying storage
    int result = kvs_base_set(kvs_fifo->kvs_base, key, value);
    if (result != 0) {
        return result;
    }
    
    // Insert the key into the cache
    if (kvs_fifo->size == kvs_fifo->capacity) {
        // Cache is full, need to evict the oldest key
        free(kvs_fifo->keys[kvs_fifo->front]);
        kvs_fifo->keys[kvs_fifo->front] = strdup(key);
        kvs_fifo->front = (kvs_fifo->front + 1) % kvs_fifo->capacity;
    } else {
        // Cache is not full, no need to evict a key
        kvs_fifo->keys[kvs_fifo->back] = strdup(key);
        kvs_fifo->size++;
    }
    kvs_fifo->back = (kvs_fifo->back + 1) % kvs_fifo->capacity;
    return 0;
}

int kvs_fifo_get(kvs_fifo_t* kvs_fifo, const char* key, char* value) {
    // The FIFO policy does not affect the get operation, so simply delegate to the underlying storage
    return kvs_base_get(kvs_fifo->kvs_base, key, value);
}

int kvs_fifo_flush(kvs_fifo_t* kvs_fifo) {
    // Clear the cache
    for (int i = 0; i < kvs_fifo->capacity; i++) {
        free(kvs_fifo->keys[i]);
        kvs_fifo->keys[i] = NULL;
    }
    kvs_fifo->size = 0;
    kvs_fifo->front = 0;
    kvs_fifo->back = 0;
    return 0;
}
