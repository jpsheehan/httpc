#include <stdlib.h>

#include "queue.h"

queue_t *queue_init(size_t size)
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

            // TODO: Check these for failures
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
    // wait for the queue to not be full, then lock it
    sem_wait(&queue->full);
    pthread_mutex_lock(&queue->lock);

    // write the new item into the queue, increment the write index (wrapping
    // if required)
    queue->data[queue->write_index] = item;
    queue->write_index = (queue->write_index + 1) % queue->size;

    // unlock the queue and signal that the queue may not be empty now
    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->empty);
}

void *queue_dequeue(queue_t *queue)
{
    void *item;

    // wait for an item to be available then lock the queue
    sem_wait(&queue->empty);
    pthread_mutex_lock(&queue->lock);

    // read the item off the queue and increment the read index (wrapping if
    // required)
    item = queue->data[queue->read_index];
    queue->read_index = (queue->read_index + 1) % queue->size;

    // unlock the queue and signal that the queue may not be full now
    pthread_mutex_unlock(&queue->lock);
    sem_post(&queue->full);

    return item;
}

void queue_destroy(queue_t *queue)
{
    // destroy the semaphores
    sem_destroy(&queue->empty);
    sem_destroy(&queue->full);

    // destroy the mutex
    pthread_mutex_destroy(&queue->lock);

    // free the array and queue data structure
    free(queue->data);
    free(queue);
}

size_t queue_length(queue_t *queue)
{
    int length;

    // get the value of the empty semaphore (this will be equal to the length
    // of the queue)
    sem_getvalue(&queue->empty, &length);

    return length;
}