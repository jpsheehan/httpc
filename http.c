#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#include "http.h"

http_t *http_init(int conn_fd)
{
    http_t *http = calloc(1, sizeof(http_t));

    if (http)
    {
        http->req = buffer_init(8192);

        if (http->req)
        {
            http->res = buffer_init(8192);

            if (http->res)
            {
                http->conn_fd = conn_fd;
                http->headers = NULL;
            }
            else
            {
                free(http->req);
                free(http);
                http = NULL;
            }
        }
        else
        {
            free(http);
            http = NULL;
        }
    }

    return http;
}

void http_destroy(http_t *http)
{
    http_close(http);

    buffer_destroy(http->req);
    buffer_destroy(http->res);

    http_headers_destroy(http->headers);

    free(http);
}

void http_read(http_t *http)
{
    buffer_recv(http->req, http->conn_fd);
    http->headers = http_headers_init(http->req->data);
}

void http_write(http_t *http, char *src, size_t nbytes)
{
    buffer_write(http->res, src, nbytes);
}

void http_flush(http_t *http)
{
    buffer_send(http->res, http->conn_fd);
    buffer_rewind(http->res);
}

void http_close(http_t *http)
{
    http_flush(http);
    close(http->conn_fd);
}