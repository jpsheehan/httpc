#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "network.h"

int network_get_socket(int port)
{
    int sock_fd, opt = 0;
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // create the socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        return NETWORK_ERR_SOCKET;
    }

    // set socket options
    if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        return NETWORK_ERR_SETSOCKOPT;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // bind the socket
    if (bind(sock_fd, (struct sockaddr *)&address, address_len) < 0)
    {
        return NETWORK_ERR_BIND;
    }

    // listen on the socket
    if (listen(sock_fd, port) < 0)
    {
        return NETWORK_ERR_LISTEN;
    }

    return sock_fd;
}
