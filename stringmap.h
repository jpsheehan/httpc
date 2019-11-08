#ifndef STRINGMAP_H_
#define STRINGMAP_H_

// TODO: FIX THIS!!!
// It needs to actually be a string map. Not a linear search through a list!

#include "list.h"

typedef struct
{
    list_t *list;
} stringmap_t;

stringmap_t *stringmap_init(void);

void stringmap_destroy(stringmap_t *sm);

char *stringmap_get(stringmap_t *sm, const char *key);

void stringmap_put(stringmap_t *sm, const char *key, const char *val);

#endif