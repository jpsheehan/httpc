#ifndef HASHMAP_H_
#define HASHMAP_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    void *key;
    void *value;
    bool alive;
} __hashmap_item_t;

typedef struct
{
    size_t reserved;
    size_t used;
    uint16_t (*hash_f)(void *);
    bool (*key_compare_f)(void *, void *);
    __hashmap_item_t **data;
} hashmap_t;

hashmap_t *hashmap_init(size_t t_capacity, uint16_t (*t_hash_f)(void *), bool (*key_comp_f)(void *, void *));

void hashmap_destroy(hashmap_t *hm);

void hashmap_put(hashmap_t *hm, void *key, void *value);

void *hashmap_get(hashmap_t *hm, void *key);

void *hashmap_delete(hashmap_t *hm, void *key);

bool hashmap_exists(hashmap_t *hm, void *key);

#endif