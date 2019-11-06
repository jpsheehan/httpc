#ifndef LIST_H_
#define LIST_H_

struct list_s
{
    void *data;
    struct list_s *next;
};

typedef struct list_s list_t;

list_t *list_init(void *data);

void list_destroy(list_t *list);

void list_destroy_all(list_t *list);

void list_push(list_t *list, void *data);

void *list_pop(list_t *list);

int list_length(list_t *list);

void list_for(list_t *list, void (*handler)(void *));

list_t *list_map(list_t *list, void *(*handler)(void *));

#endif