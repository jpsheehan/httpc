#ifndef NETWORK_H_
#define NETWORK_H_

#define NETWORK_ERR_SOCKET (-1)
#define NETWORK_ERR_BIND (-2)
#define NETWORK_ERR_LISTEN (-3)
#define NETWORK_ERR_SETSOCKOPT (-4)

int network_get_socket(int port);

#endif