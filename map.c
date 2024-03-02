// A hash map implementation that uses separate chaining [1] to resolve collisions.
//
// [1]: https://en.wikipedia.org/wiki/Hash_table#Separate_chaining

#include "map.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
static uint64_t hash(const char* key) {
    uint64_t h = 14695981039346656037U;
    for (size_t i = 0; key[i] != '\0'; i++) {
        h ^= key[i];
        h *= 1099511628211U;
    }
    return h;
}

struct entry {
    char*         key;
    void*         value;
    struct entry* next;
};

struct map {
    size_t        n_entries;
    size_t        n_buckets;
    struct entry* buckets[];
};

map* map_new(size_t n_buckets) {
    map* m = malloc(sizeof(map) + n_buckets * sizeof(struct entry*));
    if (m == NULL) {
        return NULL;
    }

    m->n_entries = 0;
    m->n_buckets = n_buckets;
    memset(m->buckets, 0, n_buckets * sizeof(struct entry*));

    return m;
}

void* map_get(const map* m, const char* key) {
    uint64_t i = hash(key) % m->n_buckets;

    for (struct entry* e = m->buckets[i]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
    }

    return NULL;
}

void* map_set(map* m, const char* key, void* value) {
    assert(value != NULL);
    uint64_t i = hash(key) % m->n_buckets;

    for (struct entry* e = m->buckets[i]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            void* prev = e->value;
            e->value   = value;
            return prev;
        }
    }

    struct entry* e = malloc(sizeof(struct entry));
    if (e == NULL) {
        return NULL;
    }

    e->key        = strdup(key);
    e->value      = value;
    e->next       = m->buckets[i];
    m->buckets[i] = e;
    m->n_entries++;

    return value;
}

void map_del(map* m, const char* key) {
    uint64_t i = hash(key) % m->n_buckets;

    struct entry* prev = NULL;
    for (struct entry* e = m->buckets[i]; e != NULL; e = e->next, prev = e) {
        if (strcmp(e->key, key) != 0) {
            continue;
        }

        if (prev == NULL) {
            m->buckets[i] = e->next;
        } else {
            prev->next = e->next;
        }

        free(e->key);
        free(e);
        m->n_entries--;

        return;
    }
}

size_t map_len(const map* m) {
    return m->n_entries;
}

void map_free(map* m) {
    for (size_t i = 0; i < m->n_buckets; i++) {
        for (struct entry* e = m->buckets[i]; e != NULL;) {
            struct entry* next = e->next;
            free(e->key);
            free(e);
            e = next;
        }
    }
    free(m);
}

void map_print_value(void* value); // defined by the user of map_print.

void map_print(map* m) {
    printf("\n%zu buckets; %zu entries\n", m->n_buckets, m->n_entries);
    for (size_t i = 0; i < m->n_buckets; i++) {
        printf("%zu:", i);
        for (struct entry* e = m->buckets[i];; e = e->next) {
            if (e == NULL) {
                printf(" NULL");
                break;
            }
            printf(" (%s:", e->key);
            map_print_value(e->value);
            printf(") ->");
        }
        printf("\n");
    }
}

struct map_iter map_iter_new(const map* m) {
    struct map_iter it = {
        .key         = NULL,
        .value       = NULL,
        ._map        = m,
        ._bucket_idx = 0,
    };
    return it;
}

bool map_iter_next(struct map_iter* it) {
    const map* m = it->_map;

    for (; it->_bucket_idx < m->n_buckets; it->_bucket_idx++) {
        size_t i = it->_bucket_idx;
        if (m->buckets[i] == NULL) {
            continue;
        }

        if (it->key == NULL) {
            it->key   = m->buckets[i]->key;
            it->value = m->buckets[i]->value;
            return true;
        }

        for (struct entry* e = m->buckets[i]; e != NULL; e = e->next) {
            if (strcmp(e->key, it->key) == 0 && e->next != NULL) {
                it->key   = e->next->key;
                it->value = e->next->value;
                return true;
            }
        }

        it->key   = NULL;
        it->value = NULL;
    }

    return false;
}
