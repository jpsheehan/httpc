#ifndef LIST_H_
#define LIST_H_

#include <stdbool.h>

/**
 * The internal representation of a list node.
 */
struct __list_s
{
    /**
     * A pointer to the next element in the linked list.
     */
    struct __list_s *next;

    /**
     * A pointer to the previous element in the linked list.
     */
    struct __list_s *prev;

    /**
     * A pointer to the actual data.
     */
    void *data;
};

/**
 * Represents a node in a double linked list.
 */
typedef struct __list_s list_t;

/**
 * Initialises a linked list node.
 * 
 * @param t_data The pointer to the data to store.
 * @returns A pointer to the linked list.
 */
list_t *list_init(void *t_data);

/**
 * Frees the memory associated with the linked list.
 * 
 * @param t_list The list.
 */
void list_destroy(list_t *t_list);

/**
 * Frees the memory associated with the list and also frees the data members of
 * each node in the list.
 * 
 * @param t_list The list.
 */
void list_destroy_all(list_t *t_list);

/**
 * Gets the first list node.
 * 
 * @param t_list The list.
 * @returns A pointer to the first node in the list or NULL upon failure.
 */
list_t *list_first_node(list_t *t_list);

/**
 * Gets the last list node.
 * 
 * @param t_list The list.
 * @returns A pointer to the last node in the list or NULL upon failure.
 */
list_t *list_last_node(list_t *t_list);

/**
 * Gets the first list item.
 * 
 * @param t_list The list.
 * @returns A pointer to the first list item.
 */
void *list_first(list_t *t_list);

/**
 * Gets the last list item.
 * 
 * @param t_list The list.
 * @returns A pointer to the last list item.
 */
void *list_last(list_t *t_list);

/**
 * Pushes an element onto the end of the list.
 * 
 * @param t_list The list.
 * @param t_data The data to push.
 */
void list_push(list_t *t_list, void *t_data);

/**
 * Pops an element off the end of the list.
 * 
 * @param t_list The list.
 * @returns The data contained in the last node.
 */
void *list_pop(list_t *t_list);

/**
 * Gets the length of the list.
 * Note that this is a linear time operation.
 * 
 * @param t_list The list.
 * @returns The length of the list.
 */
int list_length(list_t *t_list);

/**
 * Calls a function for each element in the list.
 * The function should NOT modify the list itself but can modify the data.
 * 
 * @param t_list The list.
 * @param t_handler The function to call. It should take a void pointer to the data and the index of the data within the list.
 */
void list_for(list_t *t_list, void (*t_handler)(void *, size_t));

/**
 * Calls a function for each element in the list and creates a new list based on
 * the data returned from that function.
 * 
 * @param t_list The list.
 * @param t_handler The function to call. It should take a void pointer to the data and the index of the data within the list.
 * @returns A new list based on running the function on every element of the list.
 */
list_t *list_map(list_t *t_list, void *(*t_handler)(void *, size_t));

/**
 * Calls a function for each element in the list and creates a new list that only contains items that the function returned true for.
 * 
 * @param t_list The list.
 * @param t_handler The function to call. It should return true if the item exists in the new list.
 * @returns The new filtered list.
 */
list_t *list_filter(list_t *t_list, bool (*t_handler)(void *, size_t));

#endif