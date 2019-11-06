#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>

#include "queue.h"

#define PORT 8080
#define NUM_WORKERS 4

typedef struct
{
    int sock_fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
} thread_args_t;

queue_t *connections;

void handle_connection(int thread_id, int sock_fd, struct sockaddr_in addr)
{
    char buffer[128] = {0};
    sprintf(buffer, "Hello from thread %d\n", thread_id);
    send(sock_fd, &buffer, strlen(buffer), 0);
    close(sock_fd);
}

/**
 * Connection worker thread.
 */
void *thread_worker(void *t_args)
{
    thread_args_t *args;
    int thread_id;

    thread_id = *(int *)t_args;

    while (true)
    {
        args = queue_dequeue(connections);
        handle_connection(thread_id, args->sock_fd, args->addr);
        free(args);
    }

    return NULL;
}

/**
 * Starts the server and serves forever.
 */
void start_server(void)
{
    thread_args_t *args;
    pthread_t threads[NUM_WORKERS];
    int thread_args[NUM_WORKERS];

    int server_fd, opt, i;
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // allocate space
    connections = queue_init(NUM_WORKERS);

    // start workers
    for (i = 0; i < NUM_WORKERS; ++i)
    {
        thread_args[i] = i;
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
    address.sin_port = htons(PORT);

    // bind the socket
    if (bind(server_fd, (struct sockaddr *)&address, address_len) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    listen(server_fd, NUM_WORKERS);
    printf("Listening on http://127.0.0.1:%d/\n", PORT);

    while (true)
    {
        args = malloc(sizeof(thread_args_t));

        // accept the connection
        if ((args->sock_fd = accept(server_fd, (struct sockaddr *)&args->addr, (socklen_t *)&args->addr_len)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        };

        // add the connection to the queue
        queue_enqueue(connections, args);
    }

    queue_destroy(connections);
    close(server_fd);
}

int main(int argc, char *argv[])
{
    start_server();
    return EXIT_SUCCESS;
}