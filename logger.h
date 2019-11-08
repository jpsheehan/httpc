#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <time.h>

#include "queue.h"

typedef struct
{
    queue_t *logger_queue;
    FILE *stream;
} logger_thread_args_t;

typedef enum
{
    LOG_SRC_SERVER,
    LOG_SRC_WORKER,
    LOG_SRC_DISPATCH
} log_source_t;

typedef enum
{
    LOG_LEVEL_LOG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERR
} log_level_t;

void *logger_thread(void *t_args);

/**
 * Add a log message to the queue.
 * 
 * @param logger_queue The pointer to the global logger queue.
 * @param level The log level to use.
 * @param source Where the source of the message is.
 * @param thread_id The id of the originating thread (if applicable).
 * @param text The actual information to write out.
 */
void logger_enqueue(queue_t *logger_queue, log_level_t level, log_source_t source, int thread_id, const char *text);

#define logger_log(queue, source, thread_id, text) (logger_enqueue(queue, LOG_LEVEL_LOG, source, thread_id, text))

#define logger_warn(queue, source, thread_id, text) (logger_enqueue(queue, LOG_LEVEL_WARN, source, thread_id, text))

#define logger_info(queue, source, thread_id, text) (logger_enqueue(queue, LOG_LEVEL_INFO, source, thread_id, text))

#define logger_err(queue, source, thread_id, text) (logger_enqueue(queue, LOG_LEVEL_ERR, source, thread_id, text))

#endif