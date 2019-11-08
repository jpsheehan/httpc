#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "http.h"
#include "connection.h"

#define PORT 8080

void handle_connection(connection_t *conn)
{
    http_t *http;
    char temp[64] = {0};

    http = http_init(conn->sock_fd);
    http_read(http);

    snprintf(temp, 64, "Port: %d\nDispatcher thread id: %d\nWorker thread id: %d", conn->port, conn->dispatcher_thread_id, conn->worker_thread_id);
    http_write(http, temp, strlen(temp));

    http_destroy(http);
}

int main(int argc, char *argv[])
{
    server_t *server = server_init(handle_connection);

    server_serve(server, PORT);

    server_destroy(server);

    return EXIT_SUCCESS;
}