#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include "connection.h"
#include "queue.h"

typedef struct
{
    /**
     * The id of the dispatcher thread.
     * NOTE: This is different to the pthread_id and to the thread_id of worker threads.
     */
    int thread_id;

    /**
     * This is the port the socket should listen on.
     */
    int port;

    /**
     * The pointer to the global connection queue.
     */
    queue_t *connection_queue;

    /**
     * The socket file descriptor of the server.
     */
    int sock_fd;

    /**
     * Temporary buffer for storing the client connection.
     * NOTE: Don't use this for anything outside of dispatcher.c.
     */
    connection_t *__conn;

} dispatcher_thread_args_t;

/**
 * The thread handler for the dispatcher. Supply this to `pthread_create`.
 */
void *dispatcher_thread_worker(void *args);

#endif