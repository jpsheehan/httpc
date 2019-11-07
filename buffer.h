#ifndef BUFFER_H_
#define BUFFER_H_

#include <stdlib.h>

typedef struct
{
    size_t reserved;
    size_t used;
    char *data;
} buffer_t;

buffer_t *buffer_init(int capacity);

void buffer_destroy(buffer_t *buffer);

size_t buffer_recv(buffer_t *buffer, int fd);

size_t buffer_send(buffer_t *buffer, int fd);

void buffer_write(buffer_t *buffer, char *src, size_t nbytes);

void buffer_read(buffer_t *buffer, char *dest, size_t nbytes);

void buffer_rewind(buffer_t *buffer);

#endif