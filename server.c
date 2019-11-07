#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

#include "server.h"
#include "list.h"
#include "buffer.h"

static void *thread_worker(void *);

void signal_handler(int);

void thread_cleanup(void *t_args);

static bool accepting_connections;

typedef struct
{
    int thread_id;
    server_t *server;
} thread_args_t;

typedef struct
{
    int sock_fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
    server_t *server;
} server_client_t;

server_t *server_init(int max_threads, void (*handler)(int, int, struct sockaddr_in))
{
    server_t *server = (server_t *)malloc(sizeof(server_t));

    if (server)
    {
        server->max_threads = max_threads;
        server->connection_handler = handler;
        server->connections = queue_init(max_threads);
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
    pthread_t threads[server->max_threads];
    thread_args_t thread_args[server->max_threads];

    int server_fd, opt = 0, i;
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // start workers
    for (i = 0; i < server->max_threads; ++i)
    {
        thread_args[i] = (thread_args_t){i, server};
        pthread_create(&threads[i], NULL, thread_worker, &thread_args[i]);
    }

    // create the socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, address_len) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, port);
    printf("Listening on http://127.0.0.1:%d/\n", port);

    accepting_connections = true;
    signal(SIGINT, signal_handler);

    while (accepting_connections)
    {
        client_conn = calloc(1, sizeof(server_client_t));

        // accept the connection
        if ((client_conn->sock_fd = accept(server_fd, (struct sockaddr *)&client_conn->addr, (socklen_t *)&client_conn->addr_len)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        };

        // add the connection to the queue
        queue_enqueue(server->connections, client_conn);
    }

    // cancel all threads
    for (i = 0; i < server->max_threads; ++i)
    {
        pthread_cancel(threads[i]);
    }

    // free all threads
    for (i = 0; i < server->max_threads; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    // empty the connection queue
    while (queue_length(server->connections))
    {
        client_conn = queue_dequeue(server->connections);
        free(client_conn);
    }

    close(server_fd);
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
        accepting_connections = false;
    }
}