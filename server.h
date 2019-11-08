#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>

#include "queue.h"

typedef struct
{
    int max_threads;
    void (*connection_handler)(int, int, struct sockaddr_in);
    queue_t *connections;
} server_t;

typedef struct
{
    int sock_fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
    server_t *server;
} server_client_t;

server_t *server_init(void (*handler)(int, int, struct sockaddr_in));

void server_destroy(server_t *server);

void server_serve(server_t *server, int port);

#endif