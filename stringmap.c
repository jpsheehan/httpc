
#include <stdlib.h>
#include <string.h>

#include "stringmap.h"

typedef struct
{
    char *key;
    char *val;
} sm_item_t;

stringmap_t *stringmap_init(void)
{
    stringmap_t *sm = calloc(1, sizeof(stringmap_t));

    return sm;
}

list_t *stringmap_get_list_item(stringmap_t *sm, const char *key)
{
    list_t *cur = sm->list;
    sm_item_t *item;

    while (cur)
    {
        item = (sm_item_t *)cur->data;

        if (strcmp(key, item->key) == 0)
        {
            return cur;
        }

        cur = cur->next;
    }

    return NULL;
}

void stringmap_destroy(stringmap_t *sm)
{
    list_t *cur = sm->list;
    sm_item_t *item;

    while (cur)
    {
        item = (sm_item_t *)cur->data;
        free(item->key);
        free(item->val);

        cur = cur->next;
    }

    list_destroy_all(sm->list);
    free(sm);
}

char *stringmap_get(stringmap_t *sm, const char *key)
{
    list_t *cur;
    sm_item_t *item;

    cur = stringmap_get_list_item(sm, key);

    if (cur)
    {
        item = (sm_item_t *)cur->data;
        return item->val;
    }

    return NULL;
}

void stringmap_put(stringmap_t *sm, const char *key, const char *val)
{
    list_t *cur;
    sm_item_t *item;

    cur = stringmap_get_list_item(sm, key);
    if (cur)
    {
        // update
        item = (sm_item_t *)cur->data;
        free(item->val); // remove old value
        item->val = calloc(strlen(key) + 1, sizeof(char));
        strcpy(item->val, val);
    }
    else
    {
        // add new
        item = calloc(1, sizeof(sm_item_t));

        item->key = calloc(strlen(key) + 1, sizeof(char));
        strcpy(item->key, key);

        item->val = calloc(strlen(val) + 1, sizeof(char));
        strcpy(item->val, val);

        if (sm->list == NULL)
        {
            sm->list = list_init(item);
        }
        else
        {
            list_push(sm->list, item);
        }
    }
}