#include "dining.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct dining {
    int capacity;
    int num_students;
    bool cleaning_in_progress;
    pthread_mutex_t mutex;
    pthread_cond_t student_cond;
    pthread_cond_t cleaning_cond;
} dining_t;

dining_t *dining_init(int capacity) {
    dining_t *dining = malloc(sizeof(dining_t));
    dining->capacity = capacity;
    dining->num_students = 0;
    dining->cleaning_in_progress = false;
    pthread_mutex_init(&dining->mutex, NULL);
    pthread_cond_init(&dining->student_cond, NULL);
    pthread_cond_init(&dining->cleaning_cond, NULL);

    return dining;
}

void dining_destroy(dining_t **dining) {
    pthread_mutex_destroy(&(*dining)->mutex);
    pthread_cond_destroy(&(*dining)->student_cond);
    pthread_cond_destroy(&(*dining)->cleaning_cond);
    free(*dining);
    *dining = NULL;
}

void dining_student_enter(dining_t *dining) {
    pthread_mutex_lock(&dining->mutex);

    while (dining->num_students >= dining->capacity || dining->cleaning_in_progress) {
        pthread_cond_wait(&dining->student_cond, &dining->mutex);
    }
    dining->num_students++;

    pthread_mutex_unlock(&dining->mutex);
}

void dining_student_leave(dining_t *dining) {
    pthread_mutex_lock(&dining->mutex);

    dining->num_students--;

    if (dining->num_students == 0) {
        pthread_cond_signal(&dining->cleaning_cond);
    } else {
        pthread_cond_signal(&dining->student_cond);
    }

    pthread_mutex_unlock(&dining->mutex);
}

void dining_cleaning_enter(dining_t *dining) {
    pthread_mutex_lock(&dining->mutex);

    while (dining->num_students > 0) {
        pthread_cond_wait(&dining->cleaning_cond, &dining->mutex);
    }
    dining->cleaning_in_progress = true;

    pthread_mutex_unlock(&dining->mutex);
}

void dining_cleaning_leave(dining_t *dining) {
    pthread_mutex_lock(&dining->mutex);

    dining->cleaning_in_progress = false;
    pthread_cond_broadcast(&dining->student_cond);

    pthread_mutex_unlock(&dining->mutex);
}
