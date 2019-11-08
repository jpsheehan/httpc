#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <netinet/in.h>

typedef struct
{
    /**
     * The id of the dispatcher thread that serviced this connection.
     * NOTE: This is different from the pthread id.
     */
    int dispatcher_thread_id;

    /**
     * The id of the worker thread that serviced this connection.
     * NOTE: This is different from the pthread id.
     */
    int worker_thread_id;

    /**
     * The server port of the incoming connection.
     */
    int port;

    /**
     * The client socket for this connection.
     */
    int sock_fd;

    /**
     * The full address struct of the connection.
     */
    struct sockaddr_in addr;

    /**
     * The length of the address struct.
     * TODO: Is this required?
     */
    socklen_t addr_len;
} connection_t;

#endif