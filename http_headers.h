#ifndef HTTP_HEADER_H_
#define HTTP_HEADER_H_

#include "list.h"
#include "http_method.h"

typedef struct
{
    list_t *keys;
    list_t *values;
    http_method_t method;
    char *path;
    char *version;
} http_headers_t;

http_headers_t *http_headers_init(char *buffer);

void http_headers_destroy(http_headers_t *headers);

#endif