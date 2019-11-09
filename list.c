#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

list_t *list_init(void *t_data)
{
    list_t *list = malloc(sizeof(list_t));

    if (list)
    {
        list->data = t_data;
        list->next = NULL;
    }

    return list;
}

void list_destroy(list_t *t_list)
{
    list_t *next;
    while (t_list->next)
    {
        next = t_list->next;
        free(t_list);
        t_list = next;
    }
    free(t_list);
}

void list_destroy_all(list_t *t_list)
{
    list_t *next;
    if (t_list)
    {
        while (t_list->next)
        {
            next = t_list->next;
            free(t_list->data);
            free(t_list);
            t_list = next;
        }
        free(t_list->data);
        free(t_list);
    }
}

void list_push(list_t *t_list, void *t_data)
{
    while (t_list->next)
    {
        t_list = t_list->next;
    }

    t_list->next = list_init(t_data);
}

void *list_pop(list_t *t_list)
{
    list_t *prev = NULL;
    void *data = NULL;

    if (t_list)
    {
        while (t_list->next)
        {
            prev = t_list;
            t_list = t_list->next;
        }

        data = t_list->data;
        free(t_list);

        if (prev)
        {
            prev->next = NULL;
        }
    }

    return data;
}

int list_length(list_t *t_list)
{
    int length = 0;

    if (t_list)
    {
        ++length;
        while (t_list->next)
        {
            ++length;
            t_list = t_list->next;
        }
    }

    return length;
}

void list_for(list_t *t_list, void (*handler)(void *, size_t))
{
    size_t index = 0;
    while (t_list)
    {
        handler(t_list->data, index++);
        t_list = t_list->next;
    }
}

list_t *list_map(list_t *t_list, void *(*t_handler)(void *, size_t))
{
    list_t *new_list = NULL;
    void *data;
    size_t index = 0;

    while (t_list)
    {
        data = t_handler(t_list->data, index++);

        if (new_list)
        {
            list_push(new_list, data);
        }
        else
        {
            new_list = list_init(data);
        }

        t_list = t_list->next;
    }

    return new_list;
}

// list_t *list_fold(list_t *list, void *(*handler)(void *, void *))
// {
// }