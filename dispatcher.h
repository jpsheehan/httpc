#ifndef DISPATCHER_H_
#define DISPATCHER_H_

#include "server.h"

typedef struct
{
    server_t *server;
} dispatcher_thread_args_t;

void *dispatcher_thread_worker(void *args);

#endif