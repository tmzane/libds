#include "map.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

// #define DEBUG

void map_print(map*);

void map_print_value(void* value) {
    printf("%d", *(int*)value);
}

size_t map_n_buckets(map* m) {
    return *(((size_t*)m) + 1); // n_buckets is the second field.
}

int main(void) {
    char* keys[] = {"foo", "bar", "bar", "baz", "baz", "baz"};

    map* m = map_new();
    assert(map_len(m) == 0);
    assert(map_n_buckets(m) == 8);

    for (size_t i = 0; i < 6; i++) {
        int* p = map_get(m, keys[i]);
        if (p == NULL) {
            p = calloc(1, sizeof(int));
            assert(map_set(m, keys[i], p) == p);
        }
        (*p)++;
    }

    assert(map_len(m) == 3);
    assert(*(int*)map_get(m, "foo") == 1);
    assert(*(int*)map_get(m, "bar") == 2);
    assert(*(int*)map_get(m, "baz") == 3);

    for (struct map_iter it = map_iter_new(m); map_iter_next(&it);) {
        *(int*)it.value *= 2;
    }

    assert(map_len(m) == 3);
    assert(*(int*)map_get(m, "foo") == 2);
    assert(*(int*)map_get(m, "bar") == 4);
    assert(*(int*)map_get(m, "baz") == 6);

#ifdef DEBUG
    map_print(m);
#endif

    int n1 = 1, n2 = 2;
    assert(map_set(m, "abc", &n1) == &n1);
    assert(map_set(m, "xyz", &n2) == &n2);
    assert(map_len(m) == 5);
    assert(map_n_buckets(m) == 16);

    assert(*(int*)map_get(m, "abc") == 1);
    assert(map_set(m, "abc", &n2) == &n1);
    assert(*(int*)map_get(m, "abc") == 2);

#ifdef DEBUG
    map_print(m);
#endif

    map_del(m, "abc");
    map_del(m, "xyz");
    map_del(m, "non");
    assert(map_len(m) == 3);

    for (struct map_iter it = map_iter_new(m); map_iter_next(&it);) {
        free(it.value);
    }
    map_free(m);

    return EXIT_SUCCESS;
}
