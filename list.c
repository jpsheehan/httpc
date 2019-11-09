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
        list->prev = NULL;
    }

    return list;
}

void list_destroy(list_t *t_list)
{
    list_t *cur = list_first_node(t_list);
    list_t *next;

    if (cur)
    {
        while (cur->next)
        {
            next = cur->next;
            free(cur);
            cur = next;
        }

        free(cur);
    }
}

void list_destroy_all(list_t *t_list)
{
    list_t *cur = list_first_node(t_list);
    list_t *next;

    if (cur)
    {
        while (cur->next)
        {
            next = cur->next;
            free(cur->data);
            free(cur);
            cur = next;
        }

        free(cur->data);
        free(cur);
    }
}

void list_push(list_t *t_list, void *t_data)
{
    list_t *tail = list_last_node(t_list);

    if (tail)
    {
        // link the tail in
        tail->next = list_init(t_data);
        tail->next->prev = tail;
    }
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

        // TODO: Fix case where we pop the last element off!

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

list_t *list_filter(list_t *t_list, bool (*t_handler)(void *, size_t))
{
    list_t *new_list = NULL;
    size_t offset = 0;

    while (t_list)
    {
        // add the item to the new list if the handler returns true
        if (t_handler(t_list->data, offset++))
        {
            if (new_list)
            {
                list_push(new_list, t_list->data);
            }
            else
            {
                new_list = list_init(t_list->data);
            }
        }

        // advance to the next node
        t_list = t_list->next;
    }

    return new_list;
}

list_t *list_first_node(list_t *t_list)
{
    if (t_list)
    {
        while (t_list->prev)
        {
            t_list = t_list->prev;
        }
    }
    return t_list;
}

list_t *list_last_node(list_t *t_list)
{
    if (t_list)
    {
        while (t_list->next)
        {
            t_list = t_list->next;
        }
    }
    return t_list;
}

void *list_first(list_t *t_list)
{
    void *first = NULL;
    list_t *head = list_first_node(t_list);

    if (head)
    {
        first = head->data;
    }

    return first;
}

void *list_last(list_t *t_list)
{
    void *last = NULL;
    list_t *tail = list_last_node(t_list);

    if (tail)
    {
        last = tail->data;
    }

    return last;
}

// list_t *list_fold(list_t *list, void *(*handler)(void *, void *))
// {
// }