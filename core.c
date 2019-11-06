#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"

void handle_connection(int thread_id, int sock_fd, struct sockaddr_in addr)
{
    char buffer[128] = {0};
    sprintf(buffer, "Hello from thread %d\n", thread_id);

    send(sock_fd, &buffer, strlen(buffer), 0);
    close(sock_fd);
}

int main(int argc, char *argv[])
{
    server_t *server = server_init(5, handle_connection);

    server_serve(server, 8080);

    return EXIT_SUCCESS;
}