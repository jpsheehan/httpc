#ifndef SERVER_H_
#define SERVER_H_

#include <netinet/in.h>

#include "queue.h"
#include "connection.h"

struct server_s
{
    void (*connection_handler)(connection_t *, struct server_s *);
    queue_t *connection_queue;
    queue_t *logger_queue;
};

typedef struct server_s server_t;

server_t *server_init(void (*handler)(connection_t *, server_t *));

void server_destroy(server_t *server);

void server_serve(server_t *server, uint16_t port[], uint8_t num_ports);

#endif