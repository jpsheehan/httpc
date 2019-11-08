#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "worker.h"
#include "queue.h"
#include "connection.h"
#include "logger.h"

void worker_cleanup(void *t_args)
{
    worker_thread_args_t *args = (worker_thread_args_t *)t_args;

    // TODO: Might need to free that active connection?

    // print a message
    logger_info(args->server->logger_queue, LOG_SRC_WORKER, args->thread_id, "Worker thread stopped");
}

void *worker_thread(void *t_args)
{
    connection_t *conn;
    worker_thread_args_t *args;

    args = (worker_thread_args_t *)t_args;

    pthread_cleanup_push(worker_cleanup, t_args);

    logger_info(args->server->logger_queue, LOG_SRC_WORKER, args->thread_id, "Worker thread started");

    while (true)
    {
        // dequeue the connection
        conn = queue_dequeue(args->server->connection_queue);

        // add the id of the current worker thread to it
        conn->worker_thread_id = args->thread_id;

        // call the connection handler
        (args->server->connection_handler)(conn, args->server);

        // free the connection
        free(conn);
    }

    pthread_cleanup_pop(true);

    return NULL;
}