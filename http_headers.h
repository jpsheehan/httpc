#ifndef HTTP_HEADER_H_
#define HTTP_HEADER_H_

#include "http_method.h"
#include "stringmap.h"

typedef struct
{
    stringmap_t *headers;
    http_method_t method;
    char *path;
    char *version;
} http_headers_t;

http_headers_t *http_headers_init(char *buffer);

void http_headers_destroy(http_headers_t *headers);

#endif