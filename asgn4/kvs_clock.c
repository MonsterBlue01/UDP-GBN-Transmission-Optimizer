#include "kvs_clock.h"

#include <stdlib.h>

struct kvs_clock {
    kvs_base_t* kvs_base;
    int capacity;
    int size;
    int hand; // "hand" for the clock eviction policy
    char** keys; // Array of keys
    int* ref_bits; // Array of reference bits
};

kvs_clock_t* kvs_clock_new(kvs_base_t* kvs, int capacity) {
    kvs_clock_t* kvs_clock = malloc(sizeof(kvs_clock_t));
    kvs_clock->kvs_base = kvs;
    kvs_clock->capacity = capacity;
    kvs_clock->size = 0;
    kvs_clock->hand = 0;
    kvs_clock->keys = malloc(sizeof(char*) * capacity);
    kvs_clock->ref_bits = malloc(sizeof(int) * capacity);

    // Initialize keys and reference bits
    for (int i = 0; i < capacity; ++i) {
        kvs_clock->keys[i] = NULL;
        kvs_clock->ref_bits[i] = 0;
    }

    return kvs_clock;
}

void kvs_clock_free(kvs_clock_t** ptr) {
    kvs_clock_t* kvs_clock = *ptr;
    for (int i = 0; i < kvs_clock->capacity; ++i) {
        free(kvs_clock->keys[i]);
    }
    free(kvs_clock->keys);
    free(kvs_clock->ref_bits);
    free(kvs_clock);
    *ptr = NULL;
}

int find_index(kvs_clock_t* kvs_clock, const char* key) {
    for (int i = 0; i < kvs_clock->capacity; i++) {
        if (kvs_clock->keys[i] != NULL && strcmp(kvs_clock->keys[i], key) == 0) {
            return i;
        }
    }
    return -1;
}

int kvs_clock_set(kvs_clock_t* kvs_clock, const char* key, const char* value) {
    // Set the value in the underlying storage
    int result = kvs_base_set(kvs_clock->kvs_base, key, value);
    if (result != 0) {
        return result;
    }
    
    int index = find_index(kvs_clock, key);
    if (index >= 0) {
        // Key is already in the cache. Update its reference bit.
        kvs_clock->ref_bits[index] = 1;
    } else {
        // Key is not in the cache. Evict an item if necessary, then insert it.
        if (kvs_clock->size == kvs_clock->capacity) {
            // Cache is full, need to evict an item
            while (kvs_clock->ref_bits[kvs_clock->hand] != 0) {
                // Clear the reference bit and move the hand
                kvs_clock->ref_bits[kvs_clock->hand] = 0;
                kvs_clock->hand = (kvs_clock->hand + 1) % kvs_clock->capacity;
            }
            // Now the hand points to an item we can evict
            free(kvs_clock->keys[kvs_clock->hand]);
        } else {
            // Cache is not full, no need to evict an item
            kvs_clock->size++;
        }
        // Insert the new key into the cache and set its reference bit
        kvs_clock->keys[kvs_clock->hand] = strdup(key);
        kvs_clock->ref_bits[kvs_clock->hand] = 1;
        kvs_clock->hand = (kvs_clock->hand + 1) % kvs_clock->capacity;
    }
    return 0;
}

int kvs_clock_get(kvs_clock_t* kvs_clock, const char* key, char* value) {
    int index = find_index(kvs_clock, key);
    if (index >= 0) {
        // Key is in the cache. Get its value from the underlying storage and update its reference bit.
        int result = kvs_base_get(kvs_clock->kvs_base, key, value);
        kvs_clock->ref_bits[index] = 1;
        return result;
    } else {
        // Key is not in the cache. Get its value from the underlying storage but do not insert it into the cache.
        return kvs_base_get(kvs_clock->kvs_base, key, value);
    }
}

int kvs_clock_flush(kvs_clock_t* kvs_clock) {
    // The flush operation is not required for kvs_base_t as per your description.
    // If you still want to implement this, it could clear the cache.
    for (int i = 0; i < kvs_clock->capacity; i++) {
        free(kvs_clock->keys[i]);
        kvs_clock->keys[i] = NULL;
        kvs_clock->ref_bits[i] = 0;
    }
    kvs_clock->size = 0;
    kvs_clock->hand = 0;
    return 0;
}
