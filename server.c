#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#include <stdint.h>

#include "dispatcher.h"
#include "server.h"
#include "buffer.h"
#include "worker.h"
#include "config.h"
#include "logger.h"
#include "connection.h"

static pthread_t *dispatcher_threads;
static uint8_t num_dispatcher_threads;

void signal_handler(int sig)
{
    uint8_t i;
    switch (sig)
    {
    case SIGINT:
        // cancel the dispatcher threads
        for (i = 0; i < num_dispatcher_threads; ++i)
        {
            pthread_cancel(dispatcher_threads[i]);
        }
        break;
    }
}

server_t *server_init(void (*handler)(connection_t *, server_t *))
{
    server_t *server = (server_t *)malloc(sizeof(server_t));

    if (server)
    {
        server->connection_handler = handler;
        server->connection_queue = queue_init(CONFIG_CONNECTION_QUEUE_SIZE);
        server->logger_queue = queue_init(CONFIG_LOGGER_QUEUE_SIZE);
    }

    return server;
}

void server_destroy(server_t *server)
{
    queue_destroy(server->logger_queue);
    queue_destroy(server->connection_queue);
    free(server);
}

void server_serve(server_t *server, uint16_t ports[], uint8_t num_ports)
{
    connection_t *client_conn;
    pthread_t worker_threads[CONFIG_NUM_WORKER_THREADS], logger_threads;
    worker_thread_args_t worker_args[CONFIG_NUM_WORKER_THREADS];
    dispatcher_thread_args_t dispatcher_args[num_ports];
    logger_thread_args_t logger_args;

    uint8_t i;

    // start the logging thread
    logger_args = (logger_thread_args_t){server->logger_queue, stdout};
    pthread_create(&logger_threads, NULL, &logger_thread, &logger_args);

    // start workers
    for (i = 0; i < CONFIG_NUM_WORKER_THREADS; ++i)
    {
        worker_args[i] = (worker_thread_args_t){i, server};
        pthread_create(&worker_threads[i], NULL, &worker_thread, &worker_args[i]);
    }

    // start dispatchers
    num_dispatcher_threads = num_ports;
    dispatcher_threads = calloc(num_dispatcher_threads, sizeof(pthread_t));
    for (i = 0; i < num_dispatcher_threads; ++i)
    {
        dispatcher_args[i] = (dispatcher_thread_args_t){i, ports[i], server->connection_queue, server->logger_queue, -1, NULL};
        pthread_create(&dispatcher_threads[i], NULL, &dispatcher_thread_worker, &dispatcher_args[i]);
    }

    // set the SIGINT handler
    signal(SIGINT, signal_handler);

    // wait for dispatchers to be cancelled (as a result of SIGINT, etc)
    for (i = 0; i < num_dispatcher_threads; ++i)
    {
        pthread_join(dispatcher_threads[i], NULL);
    }

    free(dispatcher_threads);

    // cancel all worker threads
    for (i = 0; i < CONFIG_NUM_WORKER_THREADS; ++i)
    {
        pthread_cancel(worker_threads[i]);
    }

    // free all worker threads
    for (i = 0; i < CONFIG_NUM_WORKER_THREADS; ++i)
    {
        pthread_join(worker_threads[i], NULL);
    }

    // empty the connection queue
    while (queue_length(server->connection_queue))
    {
        client_conn = queue_dequeue(server->connection_queue);
        free(client_conn);
    }

    // cancel and free the logging thread
    pthread_cancel(logger_threads);
    pthread_join(logger_threads, NULL);
}
