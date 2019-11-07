#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "http.h"

#define PORT 8080

void handle_connection(int thread_id, int sock_fd, struct sockaddr_in addr)
{
    http_t *http;
    char temp[32] = {0};

    http = http_init(sock_fd);
    http_read(http);

    snprintf(temp, 32, "Hello from thread %d\n", thread_id);
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