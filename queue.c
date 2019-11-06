#include <stdlib.h>

#include "queue.h"

queue_t *queue_init(int size)
{
    queue_t *queue = malloc(sizeof(queue_t));

    if (queue)
    {
        queue->data = malloc(sizeof(void *) * size);

        if (queue->data)
        {
            queue->size = size;
            queue->read_index = 0;
            queue->write_index = 0;

            pthread_mutex_init(&queue->lock, NULL);
            sem_init(&queue->empty, 0, 0);
            sem_init(&queue->full, 0, size);
        }
        else
        {
            free(queue);
            queue = NULL;
        }
    }

    return queue;
}

void queue_enqueue(queue_t *queue, void *item)
{
    sem_wait(&queue->full);
    pthread_mutex_lock(&queue->lock);

    queue->data[queue->write_index] = item;
    queue->write_index = (queue->write_index + 1) % queue->size;

    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->empty);
}

void *queue_dequeue(queue_t *queue)
{
    void *item;

    sem_wait(&queue->empty);
    pthread_mutex_lock(&queue->lock);

    item = queue->data[queue->read_index];
    queue->read_index = (queue->read_index + 1) % queue->size;

    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->full);

    return item;
}

void queue_destroy(queue_t *queue)
{
    sem_destroy(&queue->empty);
    sem_destroy(&queue->full);
    pthread_mutex_destroy(&queue->lock);
    free(queue->data);
    free(queue);
}