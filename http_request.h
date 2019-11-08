#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "connection.h"

/**
 * Represents a HTTP request.
 */
typedef struct
{
    /**
     * The underlying socket connection.
     */
    connection_t *conn;
} http_request_t;

#endif