#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>
#include <semaphore.h>

typedef struct
{
    int size;
    void **data;

    pthread_mutex_t lock;
    sem_t empty;
    sem_t full;

    int read_index;
    int write_index;
} queue_t;

queue_t *queue_init(int size);

void queue_enqueue(queue_t *queue, void *item);

void *queue_dequeue(queue_t *queue);

void queue_destroy(queue_t *queue);

int queue_length(queue_t *queue);

#endif