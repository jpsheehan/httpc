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
    if (args->__client_conn)
    {
        free(args->__client_conn);
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
        args->__client_conn = calloc(1, sizeof(server_client_t));

        // attempt to accept the connection
        if ((args->__client_conn->sock_fd = accept(args->sock_fd, (struct sockaddr *)&args->__client_conn->addr, (socklen_t *)&args->__client_conn->addr_len)) < 0)
        {
            perror("accept failed");
            free(args->__client_conn);
            continue;
        };

        // enqueue the connection
        queue_enqueue(args->connection_queue, args->__client_conn);
        args->__client_conn = NULL;
    }

    pthread_cleanup_pop(true);

    return NULL;
}
