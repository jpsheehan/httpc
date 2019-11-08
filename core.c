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
    int *ports, i;
    size_t num_ports;
    server_t *server;

    if (argc < 2)
    {
        printf("usage: %s port1 port2 ... portN\n", argv[0]);
        return EXIT_FAILURE;
    }
    else
    {
        num_ports = argc - 1;
        ports = calloc(num_ports, sizeof(int));

        for (i = 0; i < num_ports; ++i)
        {
            ports[i] = atoi(argv[i + 1]);
            if (ports[i] < 1 || ports[i] > 65535)
            {
                fprintf(stderr, "error: '%s' does not appear to be a valid port\n", argv[i + 1]);

                free(ports);
                return EXIT_FAILURE;
            }
        }

        server = server_init(handle_connection);
        server_serve(server, ports, num_ports);

        server_destroy(server);
        free(ports);

        return EXIT_SUCCESS;
    }
}