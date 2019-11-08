#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>

#include "dispatcher.h"
#include "queue.h"
#include "network.h"

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
    printf("[%d] Dispatcher thread stopped!\n", args->thread_id);
}

void *dispatcher_thread_worker(void *t_args)
{
    dispatcher_thread_args_t *args = (dispatcher_thread_args_t *)t_args;

    // add the cleanup handler
    pthread_cleanup_push(dispatcher_thread_cleanup, t_args);

    // attempt to create the socket, exiting upon failure
    if ((args->sock_fd = network_get_socket(args->port)) < 0)
    {
        perror("Couldn't open socket :(");
        pthread_exit(NULL);
    }

    // print a nice message
    printf("[%d] Dispatcher listening on http://localhost:%d/\n", args->thread_id, args->port);

    while (true)
    {
        // allocate space for the client connection
        args->__conn = calloc(1, sizeof(connection_t));

        // attempt to accept the connection
        if ((args->__conn->sock_fd = accept(args->sock_fd, (struct sockaddr *)&args->__conn->addr, (socklen_t *)&args->__conn->addr_len)) < 0)
        {
            perror("accept failed");
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
