#ifndef HASHMAP_H_
#define HASHMAP_H_

typedef struct
{
    int reserved;
    void **data;
} hashmap_t;

hashmap_t *hashmap_init(int capacity);

#endif