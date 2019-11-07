#ifndef HTTP_H_
#define HTTP_H_

#include "buffer.h"
#include "server.h"
#include "list.h"

typedef enum
{
    GET,
    POST,
    HEAD,
    CONNECT,
    OPTIONS,
    PUT,
    DELETE
} http_method_t;

typedef struct
{
    int conn_fd;
    buffer_t *req;
    buffer_t *res;
    char *path;
    http_method_t method;
    list_t *req_headers;
    char *req_body;
} http_t;

http_t *http_init(int conn_fd);

void http_destroy(http_t *http);

void http_read(http_t *http);

void http_write(http_t *http, char *src, size_t nbytes);

void http_flush(http_t *http);

void http_close(http_t *http);

#endif