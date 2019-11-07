#ifndef WORKER_H_
#define WORKER_H_

#include "server.h"

typedef struct
{
    int thread_id;
    server_t *server;
} worker_thread_args_t;

void *worker_thread(void *t_args);

#endif