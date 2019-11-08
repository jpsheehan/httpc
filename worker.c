#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "worker.h"
#include "queue.h"
#include "connection.h"

void worker_cleanup(void *t_args)
{
    worker_thread_args_t *args = (worker_thread_args_t *)t_args;

    // print a message
    printf("[%d] Worker thread stopped!\n", args->thread_id);
}

void *worker_thread(void *t_args)
{
    connection_t *conn;
    worker_thread_args_t *args;

    args = (worker_thread_args_t *)t_args;

    pthread_cleanup_push(worker_cleanup, t_args);

    printf("[%d] Worker thread started!\n", args->thread_id);

    while (true)
    {
        conn = queue_dequeue(args->server->connection_queue);
        (args->server->connection_handler)(args->thread_id, conn->sock_fd, conn->addr);
        free(conn);
    }

    pthread_cleanup_pop(true);

    return NULL;
}