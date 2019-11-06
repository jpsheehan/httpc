#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "list.h"

void handle_connection(int thread_id, int sock_fd, struct sockaddr_in addr)
{
    char buffer[128] = {0};
    sprintf(buffer, "Hello from thread %d\n", thread_id);

    send(sock_fd, &buffer, strlen(buffer), 0);
    close(sock_fd);
}

void print_numbers(void *data)
{
    int *number = (int *)data;
    printf("The number is %d\n", *number);
}

void *square_number(void *data)
{
    int *x = malloc(sizeof(int));
    *x = *(int *)data * *(int *)data;
    return x;
}

int main(int argc, char *argv[])
{
    // server_t *server = server_init(5, handle_connection);

    // server_serve(server, 8080);

    int nums[] = {0, 1, 2, 3, 4, 5};
    int i;

    list_t *list = list_init(&nums[0]), *squared_list;

    for (i = 1; i < 6; ++i)
    {
        list_push(list, &nums[i]);
    }

    int n = *(int *)list_pop(list);
    printf("Popped off %d\n", n);

    list_for(list, print_numbers);

    squared_list = list_map(list, square_number);

    list_for(squared_list, print_numbers);

    list_destroy_all(squared_list);

    list_destroy(list);

    return EXIT_SUCCESS;
}