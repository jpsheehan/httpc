#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "server.h"
#include "http.h"
#include "connection.h"
#include "list.h"

#define PORT 8080

void log_connection(FILE *file, http_t *http)
{
    time_t t;
    struct tm *temp;
    char time_str[32] = {0};

    t = time(NULL);
    temp = localtime(&t);

    strftime(time_str, sizeof(time_str), "%F %H:%M:%S", temp);

    char *method_str;

    switch (http->headers->method)
    {
    case GET:
        method_str = "GET";
        break;
    case POST:
        method_str = "POST";
        break;
    case PUT:
        method_str = "PUT";
        break;
    case DELETE:
        method_str = "DELETE";
        break;
    case HEAD:
        method_str = "HEAD";
        break;
    case CONNECT:
        method_str = "CONNECT";
        break;
    case OPTIONS:
        method_str = "OPTIONS";
        break;
    default:
        method_str = "???";
        break;
    }

    char *host;

    host = stringmap_get(http->headers->headers, "Host");

    if (host != NULL)
    {

        fprintf(file, "%s %s %s %s\n", time_str, method_str, host, http->headers->path);
    }
    else
    {
        fprintf(file, "FAILED REQUEST: %lu/%lu bytes read\n%s", http->req->used, http->req->reserved, http->req->data);
    }
}

void handle_connection(connection_t *conn)
{
    http_t *http;
    char temp[64] = {0};

    http = http_init(conn->sock_fd);
    http_read(http);

    if (http->headers)
    {
        log_connection(stdout, http);

        snprintf(temp, 64, "Port: %d\nDispatcher thread id: %d\nWorker thread id: %d", conn->port, conn->dispatcher_thread_id, conn->worker_thread_id);
        http_write(http, temp, strlen(temp));
    }

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