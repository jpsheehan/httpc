#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>

#include "queue.h"

typedef struct
{
    int max_threads;
    void (*connection_handler)(int, int, struct sockaddr_in);
    queue_t *connection_queue;
} server_t;

server_t *server_init(void (*handler)(int, int, struct sockaddr_in));

void server_destroy(server_t *server);

void server_serve(server_t *server, int port);

#endif