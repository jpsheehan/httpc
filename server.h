#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>

#include "queue.h"
#include "connection.h"

typedef struct
{
    int max_threads;
    void (*connection_handler)(connection_t *);
    queue_t *connection_queue;
} server_t;

server_t *server_init(void (*handler)(connection_t *));

void server_destroy(server_t *server);

void server_serve(server_t *server, int port);

#endif