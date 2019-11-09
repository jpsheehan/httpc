#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "list.h"

int main()
{
    list_t *l = NULL;
    int nums[] = {1, 2, 3, 4, 5};
    void *x;

    assert(list_length(l) == 0);

    l = list_init(&nums[0]);

    assert(list_length(l) == 1);

    list_push(l, &nums[1]);

    assert(list_length(l) == 2);

    x = list_pop(l);

    assert(list_length(l) == 1);

    assert(*(int *)x == nums[1]);

    x = list_pop(l);

    assert(list_length(l) == 0);

    assert(*(int *)x == nums[0]);

    list_destroy(l);
    l = NULL;

    return EXIT_SUCCESS;
}