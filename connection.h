#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netinet/in.h>

typedef struct
{
    int dispatcher_thread_id;
    int worker_thread_id;
    int port;
    int sock_fd;
    struct sockaddr_in addr;
    socklen_t addr_len;
} connection_t;

#endif