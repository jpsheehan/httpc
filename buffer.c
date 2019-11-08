#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "buffer.h"

#define TEMP_BUFFER_SIZE 8192

void buffer_realloc(buffer_t *buffer);

buffer_t *buffer_init(int capacity)
{
    buffer_t *buffer = malloc(sizeof(buffer_t));

    if (buffer)
    {
        buffer->data = calloc(capacity, sizeof(char));

        if (buffer->data)
        {
            buffer->reserved = capacity;
            buffer->used = 0;
        }
        else
        {
            free(buffer);
            buffer = NULL;
        }
    }

    return buffer;
}

void buffer_destroy(buffer_t *buffer)
{
    free(buffer->data);
    free(buffer);
}

size_t buffer_recv(buffer_t *buffer, int fd)
{
    ssize_t n;
    size_t start_used = buffer->used;
    char temp[TEMP_BUFFER_SIZE];

    while ((n = recv(fd, temp, TEMP_BUFFER_SIZE, MSG_DONTWAIT)) > 0)
    {
        buffer_write(buffer, temp, n);
    }

    return buffer->used - start_used;
}

size_t buffer_send(buffer_t *buffer, int fd)
{
    ssize_t n;
    size_t sent = 0;

    while ((n = send(fd, &buffer->data[sent], buffer->used - sent, MSG_NOSIGNAL)) > 0)
    {
        sent += n;
    }

    return sent;
}

void buffer_write(buffer_t *buffer, char *src, size_t nbytes)
{
    while (nbytes + buffer->used > buffer->reserved)
    {
        buffer_realloc(buffer);
    }

    memcpy(&buffer->data[buffer->used], src, nbytes);
    buffer->used += nbytes;
}

void buffer_read(buffer_t *buffer, char *dest, size_t nbytes)
{
    memcpy(dest, buffer->data, nbytes);
}

void buffer_rewind(buffer_t *buffer)
{
    buffer->used = 0;
}

void buffer_realloc(buffer_t *buffer)
{
    buffer->data = realloc(buffer->data, buffer->reserved * 2);
    buffer->reserved *= 2;
}