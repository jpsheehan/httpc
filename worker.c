#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "worker.h"
#include "queue.h"

void worker_cleanup(void *t_args)
{
}

void *worker_thread(void *t_args)
{
    server_client_t *conn;
    thread_args_t *args;

    args = (thread_args_t *)t_args;

    pthread_cleanup_push(worker_cleanup, t_args);

    while (true)
    {
        conn = queue_dequeue(args->server->connections);
        (args->server->connection_handler)(args->thread_id, conn->sock_fd, conn->addr);
        free(conn);
    }

    pthread_cleanup_pop(true);

    return NULL;
}