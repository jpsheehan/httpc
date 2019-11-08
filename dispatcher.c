#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "dispatcher.h"
#include "queue.h"
#include "network.h"
#include "logger.h"

#define DISPATCHER_BUFFER_SIZE 64

/**
 * Called automatically when the thread is cancelled/exited.
 */
void dispatcher_thread_cleanup(void *t_args)
{
    dispatcher_thread_args_t *args = (dispatcher_thread_args_t *)t_args;

    // free any connections that weren't on the queue
    if (args->__conn)
    {
        free(args->__conn);
    }

    // close the server socket
    close(args->sock_fd);

    // print a nice message
    logger_info(args->logger_queue, LOG_SRC_DISPATCH, args->thread_id, "Dispatcher thread stopped");
}

void *dispatcher_thread_worker(void *t_args)
{
    char buffer[DISPATCHER_BUFFER_SIZE] = {0};
    dispatcher_thread_args_t *args = (dispatcher_thread_args_t *)t_args;

    // add the cleanup handler
    pthread_cleanup_push(dispatcher_thread_cleanup, t_args);

    // attempt to create the socket, exiting upon failure
    if ((args->sock_fd = network_get_socket(args->port)) < 0)
    {
        snprintf(buffer, DISPATCHER_BUFFER_SIZE, "Dispatcher couldn't open port %d", args->port);
        logger_err(args->logger_queue, LOG_SRC_DISPATCH, args->thread_id, buffer);
        pthread_exit(NULL);
    }

    snprintf(buffer, DISPATCHER_BUFFER_SIZE, "Dispatcher thread listening on port %d", args->port);

    // print a nice message
    logger_info(args->logger_queue, LOG_SRC_DISPATCH, args->thread_id, buffer);

    while (true)
    {
        // allocate space for the client connection
        args->__conn = calloc(1, sizeof(connection_t));

        // attempt to accept the connection
        if ((args->__conn->sock_fd = accept(args->sock_fd, (struct sockaddr *)&args->__conn->addr, (socklen_t *)&args->__conn->addr_len)) < 0)
        {
            snprintf(buffer, DISPATCHER_BUFFER_SIZE, "Failed to accept connection on port %d", args->port);
            logger_warn(args->logger_queue, LOG_SRC_DISPATCH, args->thread_id, buffer);
            free(args->__conn);
            args->__conn = NULL;
            continue;
        };

        // set the other attributes of the connection
        args->__conn->port = args->port;
        args->__conn->dispatcher_thread_id = args->thread_id;

        // enqueue the connection
        queue_enqueue(args->connection_queue, args->__conn);
        args->__conn = NULL;
    }

    pthread_cleanup_pop(true);

    return NULL;
}
