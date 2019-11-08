#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "http.h"
#include "connection.h"
#include "list.h"

#define PORT 8080

void handle_connection(connection_t *conn)
{
    http_t *http;
    char temp[64] = {0};

    http = http_init(conn->sock_fd);
    http_read(http);

    printf("Path: %s, Version: %s\n", http->headers->path, http->headers->version);

    list_t *k = http->headers->keys;
    list_t *v = http->headers->values;

    while (k)
    {
        printf("%s=%s\n", (char *)k->data, (char *)v->data);
        k = k->next;
        v = v->next;
    }

    snprintf(temp, 64, "Port: %d\nDispatcher thread id: %d\nWorker thread id: %d", conn->port, conn->dispatcher_thread_id, conn->worker_thread_id);
    http_write(http, temp, strlen(temp));

    http_destroy(http);
}

int main(int argc, char *argv[])
{
    int temp_port;
    uint16_t *ports;
    size_t i, num_ports;
    server_t *server;

    if (argc < 2)
    {
        printf("usage: %s port1 port2 ... portN\n", argv[0]);
        return EXIT_FAILURE;
    }
    else
    {
        num_ports = argc - 1;
        ports = calloc(num_ports, sizeof(uint16_t));

        for (i = 0; i < num_ports; ++i)
        {
            temp_port = atoi(argv[i + 1]);
            if (temp_port < 1 || temp_port > 65535)
            {
                fprintf(stderr, "error: '%s' does not appear to be a valid port\n", argv[i + 1]);

                free(ports);
                return EXIT_FAILURE;
            }
            else
            {
                ports[i] = (uint16_t)temp_port;
            }
        }

        server = server_init(handle_connection);
        server_serve(server, ports, num_ports);

        server_destroy(server);
        free(ports);

        return EXIT_SUCCESS;
    }
}