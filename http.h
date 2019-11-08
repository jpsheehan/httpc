#ifndef HTTP_H_
#define HTTP_H_

#include "buffer.h"
#include "server.h"
#include "list.h"
#include "http_headers.h"

typedef struct
{
    int conn_fd;
    buffer_t *req;
    buffer_t *res;
    http_headers_t *headers;
} http_t;

http_t *http_init(int conn_fd);

void http_destroy(http_t *http);

void http_read(http_t *http);

void http_write(http_t *http, char *src, size_t nbytes);

void http_flush(http_t *http);

void http_close(http_t *http);

#endif