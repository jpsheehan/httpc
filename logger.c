#include <stdbool.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "logger.h"

typedef struct
{
    log_level_t level;
    log_source_t source;
    int thread_id;
    time_t timestamp;
    char *text;
} log_msg_t;

void logger_write(FILE *stream, log_msg_t *msg)
{
    struct tm *temp;
    char time_str[32] = {0};
    char *src_str;
    char *lvl_str;

    // get the time info
    temp = localtime(&msg->timestamp);
    strftime(time_str, sizeof(time_str), "%F %H:%M:%S", temp);

    // get the level string
    switch (msg->level)
    {
    case LOG_LEVEL_LOG:
        lvl_str = "LOG";
        break;
    case LOG_LEVEL_INFO:
        lvl_str = "INFO";
        break;
    case LOG_LEVEL_WARN:
        lvl_str = "WARN";
        break;
    case LOG_LEVEL_ERR:
        lvl_str = "ERR";
        break;
    default:
        lvl_str = "???";
        break;
    }

    // get the source string
    switch (msg->source)
    {
    case LOG_SRC_SERVER:
        src_str = "SRV";
        break;
    case LOG_SRC_DISPATCH:
        src_str = "DSP";
        break;
    case LOG_SRC_WORKER:
        src_str = "WRK";
        break;
    default:
        src_str = "???";
        break;
    }

    // print the message out
    fprintf(stream, "[%s] %s (%s/%d) %s\n", lvl_str, time_str, src_str, msg->thread_id, msg->text);
}

void logger_cleanup(void *t_args)
{
    logger_thread_args_t *args = (logger_thread_args_t *)t_args;
    log_msg_t *msg;

    // empty the queue by processing what's left
    while (queue_length(args->logger_queue))
    {
        msg = queue_dequeue(args->logger_queue);

        // log the message
        logger_write(args->stream, msg);

        // free the message
        free(msg->text);
        free(msg);
    }
}

void *logger_thread(void *t_args)
{
    logger_thread_args_t *args = (logger_thread_args_t *)t_args;
    log_msg_t *msg;

    pthread_cleanup_push(&logger_cleanup, t_args);

    while (true)
    {
        // get the message from the queue
        msg = queue_dequeue(args->logger_queue);

        // log the message
        logger_write(args->stream, msg);

        // free the message
        free(msg->text);
        free(msg);
    }

    pthread_cleanup_pop(true);

    return NULL;
}

void logger_enqueue(queue_t *logger_queue, log_level_t level, log_source_t source, int thread_id, const char *text)
{
    log_msg_t *msg = calloc(1, sizeof(log_msg_t));

    msg->level = level;
    msg->source = source;
    msg->thread_id = thread_id;
    msg->text = calloc(strlen(text) + 1, sizeof(char));
    strcpy(msg->text, text);
    msg->timestamp = time(NULL);

    queue_enqueue(logger_queue, msg);
}