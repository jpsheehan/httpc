
#include <stdlib.h>
#include <string.h>

#include "http_headers.h"

http_method_t get_method(char *buffer, size_t n)
{
    http_method_t method = UNKNOWN;
    buffer[n] = '\0';

    if (strcmp(buffer, "GET") == 0)
    {
        method = GET;
    }

    else if (strcmp(buffer, "POST") == 0)
    {
        method = POST;
    }

    else if (strcmp(buffer, "HEAD") == 0)
    {
        method = HEAD;
    }

    else if (strcmp(buffer, "DELETE") == 0)
    {
        method = DELETE;
    }

    else if (strcmp(buffer, "OPTIONS") == 0)
    {
        method = OPTIONS;
    }

    else if (strcmp(buffer, "CONNECT") == 0)
    {
        method = CONNECT;
    }

    else if (strcmp(buffer, "PUT") == 0)
    {
        method = PUT;
    }

    buffer[n] = ' ';

    return method;
}

void http_headers_parse(http_headers_t *headers, char *buffer)
{
    char *eow, *eol, *key, *val;
    size_t key_len, val_len;

    // find the end of the first word (our method)
    eow = strchr(buffer, ' ');

    if (eow)
    {
        headers->method = get_method(buffer, eow - buffer);

        if (headers->method)
        {
            buffer = eow + 1;
            eow = strchr(buffer, ' ');

            headers->path = calloc(1, eow - buffer + 1);
            strncpy(headers->path, buffer, eow - buffer);

            buffer = eow + 1;
            eow = strstr(buffer, "\r\n");

            headers->version = calloc(1, eow - buffer + 1);
            strncpy(headers->version, buffer, eow - buffer);

            buffer = eow + 2;
            eol = strstr(buffer, "\r\n");
            while (eol != NULL && buffer != eol)
            {
                eow = strchr(buffer, ':');

                if (eow)
                {
                    // seperate the key and value
                    *eow = '\0';

                    key_len = strlen(buffer);
                    key = calloc(key_len + 1, sizeof(char));
                    strncpy(key, buffer, key_len);

                    // restore the colon character
                    *eow = ':';

                    buffer = eow + 1 + 1; // advance past the colon and space
                    eow = eol;
                    *eow = '\0';

                    val_len = strlen(buffer);
                    val = calloc(val_len + 1, sizeof(char));
                    strncpy(val, buffer, val_len);

                    *eow = '\r'; // restore line feed

                    stringmap_put(headers->headers, key, val);

                    free(key);
                    free(val);
                }

                buffer = eol + 2;
                eol = strstr(buffer, "\r\n");
            }
        }
        else
        {
            // invalid method
        }
    }
    else
    {
        // no method found!
    }
}

http_headers_t *http_headers_init(char *buffer)
{
    http_headers_t *headers = calloc(1, sizeof(http_headers_t));

    if (headers)
    {
        headers->headers = stringmap_init();
        http_headers_parse(headers, buffer);
    }

    return headers;
}

void http_headers_destroy(http_headers_t *headers)
{
    stringmap_destroy(headers->headers);
    free(headers->version);
    free(headers->path);
    free(headers);
}