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

#define INIT_N_BUCKETS 8

static uint64_t hash(const char* key);
static map*     map_resize(map* m, size_t n_buckets);

struct entry {
    char*         key;
    void*         value;
    struct entry* next;
};

struct map {
    size_t         n_entries;
    size_t         n_buckets;
    struct entry** buckets;
};

map* map_new(void) {
    map* m = calloc(1, sizeof(map));
    if (m == NULL) {
        return NULL;
    }
    return map_resize(m, INIT_N_BUCKETS);
}

void* map_get(const map* m, const char* key) {
    size_t i = hash(key) % m->n_buckets;

    for (struct entry* e = m->buckets[i]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
    }

    return NULL;
}

void* map_set(map* m, const char* key, void* value) {
    assert(value != NULL);

    // TODO: shrink
    if (m->n_entries >= m->n_buckets / 2) {
        map_resize(m, m->n_buckets * 2);
    }

    size_t i = hash(key) % m->n_buckets;

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
    size_t i = hash(key) % m->n_buckets;

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

static map* map_resize(map* m, size_t n_buckets) {
    struct entry** new_buckets = calloc(n_buckets, sizeof(struct entry*));
    if (new_buckets == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < m->n_buckets; i++) {
        struct entry* next = NULL;
        for (struct entry* e = m->buckets[i]; e != NULL; e = next) {
            next           = e->next;
            size_t j       = hash(e->key) % n_buckets;
            e->next        = new_buckets[j];
            new_buckets[j] = e;
        }
    }

    free(m->buckets);
    m->n_buckets = n_buckets;
    m->buckets   = new_buckets;

    return m;
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
    free(m->buckets);
    free(m);
}

void map_print(map* m, void print_value(void*)) {
    printf("%zu buckets; %zu entries\n", m->n_buckets, m->n_entries);
    for (size_t i = 0; i < m->n_buckets; i++) {
        printf("%zu:", i);
        for (struct entry* e = m->buckets[i];; e = e->next) {
            if (e == NULL) {
                printf(" NULL");
                break;
            }
            printf(" (%s:", e->key);
            print_value(e->value);
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

// https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
static uint64_t hash(const char* key) {
    uint64_t h = 14695981039346656037U;
    for (size_t i = 0; key[i] != '\0'; i++) {
        h ^= key[i];
        h *= 1099511628211U;
    }
    return h;
}
