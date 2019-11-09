#include <stdio.h>

#include "hashmap.h"

size_t __hashmap_get_item_index(hashmap_t *t_hm, void *t_key);

hashmap_t *hashmap_init(size_t t_capacity, uint16_t (*t_hash_f)(void *), bool (*t_key_comp_f)(void *, void *))
{
    hashmap_t *hm = malloc(sizeof(hashmap_t));

    if (hm)
    {
        hm->data = calloc(t_capacity, sizeof(__hashmap_item_t *));

        if (hm->data)
        {
            hm->reserved = t_capacity;
            hm->used = 0;
            hm->hash_f = t_hash_f;
            hm->key_compare_f = t_key_comp_f;
        }
        else
        {
            free(hm);
            hm = NULL;
        }
    }

    return hm;
}

void hashmap_destroy(hashmap_t *t_hm)
{
    size_t i;

    for (i = 0; i < t_hm->reserved; ++i)
    {
        if (t_hm->data[i])
        {
            free(t_hm->data[i]);
        }
    }
    free(t_hm->data);
    free(t_hm);
}

void hashmap_put(hashmap_t *t_hm, void *t_key, void *t_value)
{
    size_t index;

    // we need to consider several cases:
    // - item doesn't exist
    // - item exists and is alive
    // - item exists and is dead

    // find the index
    index = __hashmap_get_item_index(t_hm, t_key);

    // create the slot if it doesn't exist
    if (!t_hm->data[index])
    {
        t_hm->data[index] = malloc(sizeof(__hashmap_item_t));
        t_hm->data[index]->alive = true;
        t_hm->data[index]->key = t_key;
        t_hm->used++;
    }

    // assign the value
    t_hm->data[index]->value = t_value;
}

void *hashmap_get(hashmap_t *t_hm, void *t_key)
{
    size_t index;

    index = __hashmap_get_item_index(t_hm, t_key);

    if (t_hm->data[index])
    {
        return t_hm->data[index]->value;
    }

    return NULL;
}

void *hashmap_delete(hashmap_t *t_hm, void *t_key)
{
    size_t index;
    index = __hashmap_get_item_index(t_hm, t_key);
    if (t_hm->data[index])
    {
        t_hm->data[index]->alive = false;
        return t_hm->data[index]->value;
    }
    return NULL;
}

bool hashmap_exists(hashmap_t *t_hm, void *t_key)
{
    size_t index;
    index = __hashmap_get_item_index(t_hm, t_key);
    return (t_hm->data[index] != NULL);
}

size_t __hashmap_get_item_index(hashmap_t *t_hm, void *t_key)
{
    size_t index;
    uint16_t hash;

    // calculate the hash
    hash = (t_hm->hash_f)(t_key);
    index = hash % t_hm->reserved;

    while (true)
    {
        if (t_hm->data[index])
        {
            // an item exists in the slot
            if (t_hm->key_compare_f(t_key, t_hm->data[index]->key) && t_hm->data[index]->alive)
            {
                // we have found the correct slot
                break;
            }
            else
            {
                // advance to the next slot
                if (t_hm->reserved == t_hm->used)
                {
                    perror("TODO: FIX ME (hashmap capacity == used)\n");
                }
            }
        }
        else
        {
            // there is no item in the slot. slot found!
            break;
        }

        index = (index + 1) % t_hm->reserved; // advance to the next slot
    }

    return index;
}