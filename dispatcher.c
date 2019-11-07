#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "dispatcher.h"
#include "queue.h"

static server_client_t *client_conn;

void dispatcher_thread_cleanup(void *t_args)
{
    dispatcher_thread_args_t *args = (dispatcher_thread_args_t *)t_args;

    if (client_conn)
    {
        free(client_conn);
        client_conn = NULL;
    }

    printf("[%d] Dispatcher thread stopped!\n", args->thread_id);
}

void *dispatcher_thread_worker(void *t_args)
{
    dispatcher_thread_args_t *args = (dispatcher_thread_args_t *)t_args;

    pthread_cleanup_push(dispatcher_thread_cleanup, t_args);

    printf("[%d] Dispatcher thread started!\n", args->thread_id);

    while (true)
    {
        // allocate space for the client connection
        client_conn = calloc(1, sizeof(server_client_t));

        // accept the connection
        if ((client_conn->sock_fd = accept(args->server->sock_fd, (struct sockaddr *)&client_conn->addr, (socklen_t *)&client_conn->addr_len)) < 0)
        {
            perror("accept failed");
            break;
        };

        // enqueue the connection
        queue_enqueue(args->server->connections, client_conn);
    }

    pthread_cleanup_pop(true);

    return NULL;
}
