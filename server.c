#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "dispatcher.h"
#include "server.h"
#include "list.h"
#include "buffer.h"
#include "worker.h"

static pthread_t dispatcher_thread;

void signal_handler(int sig)
{
    switch (sig)
    {
    case SIGINT:
        // cancel the dispatcher thread
        pthread_cancel(dispatcher_thread);
        break;
    }
}

server_t *server_init(int max_threads, void (*handler)(int, int, struct sockaddr_in))
{
    server_t *server = (server_t *)malloc(sizeof(server_t));

    if (server)
    {
        server->max_threads = max_threads;
        server->connection_handler = handler;
        server->connections = queue_init(max_threads);
        server->sock_fd = -1;
    }

    return server;
}

void server_destroy(server_t *server)
{
    queue_destroy(server->connections);
    free(server);
}

void server_serve(server_t *server, int port)
{
    server_client_t *client_conn;
    pthread_t worker_threads[server->max_threads];
    thread_args_t worker_args[server->max_threads];
    dispatcher_thread_args_t dispatcher_args;

    int opt = 0, i;
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // start workers
    for (i = 0; i < server->max_threads; ++i)
    {
        worker_args[i] = (thread_args_t){i, server};
        pthread_create(&worker_threads[i], NULL, &worker_thread, &worker_args[i]);
    }

    // create the socket
    if ((server->sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(server->sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind the socket
    if (bind(server->sock_fd, (struct sockaddr *)&address, address_len) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // listen on the socket
    listen(server->sock_fd, port);

    // start dispatcher
    dispatcher_args.server = server;
    pthread_create(&dispatcher_thread, NULL, &dispatcher_thread_worker, &dispatcher_args);

    // print a nice message and set the SIGINT handler
    printf("Listening on http://127.0.0.1:%d/\n", port);
    signal(SIGINT, signal_handler);

    // wait for dispatcher to be cancelled (as a result of SIGINT, etc)
    pthread_join(dispatcher_thread, NULL);

    // cancel all worker threads
    for (i = 0; i < server->max_threads; ++i)
    {
        pthread_cancel(worker_threads[i]);
    }

    // free all worker threads
    for (i = 0; i < server->max_threads; ++i)
    {
        pthread_join(worker_threads[i], NULL);
    }

    // empty the connection queue
    while (queue_length(server->connections))
    {
        client_conn = queue_dequeue(server->connections);
        free(client_conn);
    }

    // close the server connection
    close(server->sock_fd);
    server->sock_fd = -1;
}
