#include "map.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char* keys[] = {"foo", "bar", "bar", "baz", "baz", "baz"};

    map* m = map_new(3);
    assert(map_len(m) == 0);

    for (size_t i = 0; i < 6; i++) {
        int* p = map_get(m, keys[i]);
        if (p == NULL) {
            p = calloc(1, sizeof(int));
            map_set(m, keys[i], p);
        }
        (*p)++;
    }

    assert(map_len(m) == 3);
    assert(*(int*)map_get(m, "foo") == 1);
    assert(*(int*)map_get(m, "bar") == 2);
    assert(*(int*)map_get(m, "baz") == 3);

    int n = 4;
    map_set(m, "xyz", &n);
    assert(map_len(m) == 4);

    for (struct map_iter it = map_iter_new(m); map_iter_next(&it);) {
        *(int*)it.value *= 2;
    }

    assert(*(int*)map_get(m, "foo") == 2);
    assert(*(int*)map_get(m, "bar") == 4);
    assert(*(int*)map_get(m, "baz") == 6);
    assert(*(int*)map_get(m, "xyz") == 8);

    map_del(m, "xyz");
    map_del(m, "zyx");
    assert(map_len(m) == 3);

    for (struct map_iter it = map_iter_new(m); map_iter_next(&it);) {
        free(it.value);
    }
    map_free(m);

    return EXIT_SUCCESS;
}
