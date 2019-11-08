#ifndef QUEUE_H_
#define QUEUE_H_

#include <pthread.h>
#include <semaphore.h>

/**
 * The queue struct.
 * Don't modify the members directly! Use the functions in queue.h instead.
 */
typedef struct
{
    /**
     * The capacity of the queue.
     */
    size_t size;

    /**
     * The pointer array pointer.
     */
    void **data;

    /**
     * The queue lock.
     */
    pthread_mutex_t lock;

    /**
     * The empty semaphore.
     * Will block when the queue is empty.
     */
    sem_t empty;

    /**
     * The full semaphore.
     * Will block when the queue is full.
     */
    sem_t full;

    /**
     * The read index of the queue.
     */
    size_t read_index;

    /**
     * The write index of the queue.
     */
    size_t write_index;
} queue_t;

/**
 * Initialises the queue.
 * 
 * @param size The capacity of the new queue.
 * @returns A pointer to the new queue or NULL upon failure.
 */
queue_t *queue_init(size_t size);

/**
 * Enqueues an item onto the queue.
 * This will block if the queue is full.
 * 
 * @param queue The pointer to the queue.
 * @param item The item to enqueue.
 */
void queue_enqueue(queue_t *queue, void *item);

/**
 * Dequeues an item from the queue.
 * This will block if the queue is empty.
 * 
 * @param queue The pointer to the queue.
 * @returns The pointer to the dequeued item.
 */
void *queue_dequeue(queue_t *queue);

/**
 * Destroys a queue.
 * The queue must not be in use at this point and the caller is responsible for
 * freeing any pointers to allocated memory inside the queue.
 * 
 * @param queue The pointer to the queue.
 */
void queue_destroy(queue_t *queue);

/**
 * Gets the number of enqueued items.
 * NOTE: This isn't atomic so don't use this function if the queue is in use!
 * 
 * @param queue The pointer to the queue.
 * @returns The number of items in the queue.
 */
size_t queue_length(queue_t *queue);

#endif