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

static void *thread_worker(void *);

void signal_handler(int);

void thread_cleanup(void *t_args);

static pthread_t dispatcher_thread;

typedef struct
{
    int thread_id;
    server_t *server;
} thread_args_t;

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
    thread_args_t thread_args[server->max_threads];
    dispatcher_thread_args_t dispatcher_args;

    int opt = 0, i;
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // start workers
    for (i = 0; i < server->max_threads; ++i)
    {
        thread_args[i] = (thread_args_t){i, server};
        pthread_create(&worker_threads[i], NULL, thread_worker, &thread_args[i]);
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

/**
 * Connection worker thread.
 */
void *thread_worker(void *t_args)
{
    server_client_t *conn;
    thread_args_t *args;

    args = (thread_args_t *)t_args;

    pthread_cleanup_push(thread_cleanup, t_args);

    while (true)
    {
        conn = queue_dequeue(args->server->connections);
        (args->server->connection_handler)(args->thread_id, conn->sock_fd, conn->addr);
        free(conn);
    }

    pthread_cleanup_pop(true);

    return NULL;
}

void thread_cleanup(void *args)
{
    // free the args if they aren't freed
}

void signal_handler(int sig)
{
    if (sig == SIGINT)
    {
        // cancel all threads
        pthread_cancel(dispatcher_thread);
    }
}