#ifndef HTTP_METHOD_H_
#define HTTP_METHOD_H_

typedef enum
{
    UNKNOWN,
    GET,
    POST,
    PUT,
    DELETE,
    HEAD,
    CONNECT,
    OPTIONS
} http_method_t;

#endif