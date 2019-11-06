#include <stdlib.h>
#include <stdbool.h>

#include "list.h"

list_t *list_init(void *data)
{
    list_t *list = malloc(sizeof(list_t));

    if (list)
    {
        list->data = data;
        list->next = NULL;
    }

    return list;
}

void list_destroy(list_t *list)
{
    list_t *next;
    while (list->next)
    {
        next = list->next;
        free(list);
        list = next;
    }
    free(list);
}

void list_destroy_all(list_t *list)
{
    list_t *next;
    while (list->next)
    {
        next = list->next;
        free(list->data);
        free(list);
        list = next;
    }
    free(list->data);
    free(list);
}

void list_push(list_t *list, void *data)
{
    while (list->next)
    {
        list = list->next;
    }

    list->next = list_init(data);
}

void *list_pop(list_t *list)
{
    list_t *prev = NULL;
    void *data = NULL;

    if (list)
    {
        while (list->next)
        {
            prev = list;
            list = list->next;
        }

        data = list->data;
        free(list);

        if (prev)
        {
            prev->next = NULL;
        }
    }

    return data;
}

int list_length(list_t *list)
{
    int length = 0;

    if (list)
    {
        ++length;
        while (list->next)
        {
            ++length;
            list = list->next;
        }
    }

    return length;
}

void list_for(list_t *list, void (*handler)(void *))
{
    while (list)
    {
        handler(list->data);
        list = list->next;
    }
}

list_t *list_map(list_t *list, void *(*handler)(void *))
{
    list_t *new_list = NULL;
    void *data;

    while (list)
    {
        data = handler(list->data);

        if (new_list)
        {
            list_push(new_list, data);
        }
        else
        {
            new_list = list_init(data);
        }

        list = list->next;
    }

    return new_list;
}

// list_t *list_fold(list_t *list, void *(*handler)(void *, void *))
// {
// }