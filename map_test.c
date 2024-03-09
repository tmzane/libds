#include "map.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern const size_t INIT_N_BUCKETS;

extern size_t max_n_entries(size_t n_buckets);
extern size_t map_n_buckets(const map* m);
extern void   map_print(const map* m, void print_value(void*));

void print_int(void* value) { printf("%d", *(int*)value); }

void print_str(void* value) { printf("%s", (char*)value); }

void test_map_new(void) {
    map* m = map_new();
    assert(m != NULL);
    assert(map_len(m) == 0);
    assert(map_n_buckets(m) == INIT_N_BUCKETS);
    map_free(m);
}

void test_map_get_set(void) {
    map* m = map_new();
    assert(map_get(m, "nil") == NULL);

    char* keys[] = {"foo", "bar", "baz"};

    int values1[] = {1, 2, 3};
    for (size_t i = 0; i < 3; i++) {
        assert(map_set(m, keys[i], &values1[i]) == &values1[i]);
    }

    assert(map_len(m) == 3);
    assert(*(int*)map_get(m, "foo") == 1);
    assert(*(int*)map_get(m, "bar") == 2);
    assert(*(int*)map_get(m, "baz") == 3);

#ifdef DEBUG_MAP_GET_SET
    map_print(m, print_int);
#endif

    int values2[] = {-1, -2, -3};
    for (size_t i = 0; i < 3; i++) {
        assert(map_set(m, keys[i], &values2[i]) == &values1[i]);
    }

    assert(map_len(m) == 3);
    assert(*(int*)map_get(m, "foo") == -1);
    assert(*(int*)map_get(m, "bar") == -2);
    assert(*(int*)map_get(m, "baz") == -3);

#ifdef DEBUG_MAP_GET_SET
    map_print(m, print_int);
#endif

    map_free(m);
}

void test_map_del(void) {
    map* m = map_new();

    char* value = "bar";
    map_set(m, "foo", value);
    assert(map_len(m) == 1);

    assert(map_del(m, "nil") == NULL);
    assert(map_len(m) == 1);

#ifdef DEBUG_MAP_DEL
    map_print(m, print_str);
#endif

    assert(map_del(m, "foo") == value);
    assert(map_len(m) == 0);

#ifdef DEBUG_MAP_DEL
    map_print(m, print_str);
#endif

    map_free(m);
}

void test_map_resize(void) {
    map* m = map_new();

    size_t n_entries = max_n_entries(INIT_N_BUCKETS);
    char*  keys      = calloc(n_entries, 4 + 1); // keyN + \0
    char*  next_key  = keys;

    for (size_t i = 0; i < n_entries; i++) {
        (void)snprintf(next_key, 4 + 1, "key%zu", i + 1);
        map_set(m, next_key, "val");
        next_key++;
    }

    assert(map_n_buckets(m) == INIT_N_BUCKETS);

#ifdef DEBUG_MAP_RESIZE
    map_print(m, print_str);
#endif

    map_set(m, "key0", "val");
    assert(map_n_buckets(m) == INIT_N_BUCKETS * 2);

#ifdef DEBUG_MAP_RESIZE
    map_print(m, print_str);
#endif

    free(keys);
    map_free(m);
}

void test_map_iter(void) {
    map* m = map_new();

    char* key   = "foo";
    char* value = "bar";
    map_set(m, key, &value);

    struct map_iter it = map_iter_new(m);
    assert(it.key == NULL);
    assert(it.value == NULL);
    assert(it._map == m);
    assert(it._bucket_idx == 0);

    assert(map_iter_next(&it));
    assert(strcmp(it.key, "foo") == 0);
    assert(it.value == &value);
    assert(!map_iter_next(&it));

    map_free(m);
}

int main(void) {
    test_map_new();
    test_map_get_set();
    test_map_del();
    test_map_resize();
    test_map_iter();

    return EXIT_SUCCESS;
}
