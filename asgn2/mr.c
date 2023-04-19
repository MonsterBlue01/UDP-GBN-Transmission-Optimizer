#include "mr.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"
#include "kvlist.h"

void map_reduce(mapper_t mapper, size_t num_mapper, reducer_t reducer, size_t num_reducer, kvlist_t* input, kvlist_t* output) {
    kvlist_t** lists = (kvlist_t**)malloc(num_mapper * sizeof(kvlist_t*));
    for (size_t i = 0; i < num_mapper; i++) {
        lists[i] = kvlist_new();
    }
    // assign each key-value pair to a different list in the array based on its hash value
    kvlist_iterator_t* itor = kvlist_iterator_new(input);
    for (;;) {
        kvpair_t* pair = kvlist_iterator_next(itor);
        if (pair == NULL) {
            break;
        }
        unsigned long h = hash(pair->key);
        size_t index = h % num_mapper;
        kvlist_append(lists[index], kvpair_clone(pair));
    }
    kvlist_iterator_free(&itor);
    // TODO: call mapper function on each list in parallel

    // TODO: merge the output lists from the mappers

    // cleanup
    for (size_t i = 0; i < num_mapper; i++) {
        kvlist_free(&lists[i]);
    }
    free(lists);
}
