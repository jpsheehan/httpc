#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "buffer.h"

void handle_connection(int thread_id, int sock_fd, struct sockaddr_in addr)
{
    buffer_t *buff = buffer_init(1024);
    char temp[32] = {0};

    buffer_recv(buff, sock_fd);
    printf("%s", (char *)buff->data);

    buffer_rewind(buff);

    snprintf(temp, 32, "Hello from thread %d\n", thread_id);
    buffer_write(buff, temp, strlen(temp));
    buffer_send(buff, sock_fd);

    close(sock_fd);
    buffer_destroy(buff);
}

int main(int argc, char *argv[])
{
    server_t *server = server_init(5, handle_connection);

    server_serve(server, 8080);

    return EXIT_SUCCESS;
}