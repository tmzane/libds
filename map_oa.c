// A hash map implementation that uses open addressing [1] to resolve collisions.
//
// Initial number of buckets is 8.
// Maximum load factor is 0.75.
//
// [1]: https://en.wikipedia.org/wiki/Hash_table#Open_addressing

#include "map.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const size_t INIT_N_BUCKETS = 8;

size_t max_n_entries(size_t n_buckets) {
    return n_buckets / 4 * 3;
}

static uint64_t hash(const char* key);
static map*     map_resize(map* m, size_t n_buckets);

static char* tombstone = "";

struct entry {
    char* key;
    void* value;
};

struct map {
    size_t        n_entries;
    size_t        n_tombstones;
    size_t        n_buckets;
    struct entry* buckets;
};

map* map_new(void) {
    map* m = calloc(1, sizeof(map));
    if (m == NULL) {
        return NULL;
    }
    if (map_resize(m, INIT_N_BUCKETS) == NULL) {
        free(m);
        return NULL;
    }
    return m;
}

void* map_get(const map* m, const char* key) {
    assert(m != NULL);
    assert(key != NULL);

    for (size_t i = hash(key) % m->n_buckets;; i = (i + 1) % m->n_buckets) {
        struct entry e = m->buckets[i];
        if (e.key == NULL) {
            return NULL;
        }
        if (e.key == tombstone) {
            continue;
        }
        if (strcmp(e.key, key) == 0) {
            return e.value;
        }
    }
}

void* map_set(map* m, const char* key, const void* value) {
    assert(m != NULL);
    assert(key != NULL);
    assert(value != NULL);

    struct entry* e;
    for (size_t i = hash(key) % m->n_buckets;; i = (i + 1) % m->n_buckets) {
        e = &m->buckets[i];
        if (e->key == NULL) {
            break;
        }
        if (e->key == tombstone) {
            m->n_tombstones--;
            break;
        }
        if (strcmp(e->key, key) == 0) {
            void* prev = e->value;
            e->value   = (void*)value;
            return prev;
        }
    }

    e->key   = strdup(key);
    e->value = (void*)value;
    m->n_entries++;

    if (m->n_entries + m->n_tombstones > max_n_entries(m->n_buckets)) {
        if (map_resize(m, m->n_buckets * 2) == NULL) {
            return NULL;
        }
    }

    return (void*)value;
}

void* map_del(map* m, const char* key) {
    assert(m != NULL);
    assert(key != NULL);

    struct entry* e;
    for (size_t i = hash(key) % m->n_buckets;; i = (i + 1) % m->n_buckets) {
        e = &m->buckets[i];
        if (e->key == NULL) {
            return NULL;
        }
        if (e->key == tombstone) {
            continue;
        }
        if (strcmp(e->key, key) == 0) {
            break;
        }
    }

    void* value = e->value;
    free(e->key);
    e->key   = tombstone;
    e->value = NULL;
    m->n_entries--;
    m->n_tombstones++;

    return value;
}

size_t map_len(const map* m) {
    assert(m != NULL);
    return m->n_entries;
}

void map_free(map* m) {
    assert(m != NULL);
    for (size_t i = 0; i < m->n_buckets; i++) {
        if (m->buckets[i].key != tombstone) {
            free(m->buckets[i].key);
        }
    }
    free(m->buckets);
    free(m);
}

static map* map_resize(map* m, size_t n_buckets) {
    struct entry* new_buckets = calloc(n_buckets, sizeof(struct entry));
    if (new_buckets == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < m->n_buckets; i++) {
        struct entry e = m->buckets[i];
        if (e.key == NULL || e.key == tombstone) {
            continue;
        }
        for (size_t j = hash(e.key) % n_buckets;; j = (j + 1) % n_buckets) {
            if (new_buckets[j].key == NULL) {
                new_buckets[j] = e;
                break;
            }
        }
    }

    free(m->buckets);
    m->n_tombstones = 0;
    m->n_buckets    = n_buckets;
    m->buckets      = new_buckets;

    return m;
}

size_t map_n_buckets(const map* m) {
    return m->n_buckets;
}

void map_print(const map* m, void print_value(void*)) {
    printf("\n%zu buckets; %zu entries; %zu tombstones\n", m->n_buckets, m->n_entries, m->n_tombstones);
    for (size_t i = 0; i < m->n_buckets; i++) {
        struct entry e = m->buckets[i];
        printf("%zu: ", i);
        if (e.key == NULL) {
            printf("empty\n");
            continue;
        }
        if (e.key == tombstone) {
            printf("tombstone\n");
            continue;
        }
        printf("%s=", e.key);
        print_value(e.value);
        printf("\n");
    }
}

struct map_iter map_iter_new(const map* m) {
    assert(m != NULL);
    struct map_iter it = {
        .key         = NULL,
        .value       = NULL,
        ._map        = (map*)m,
        ._bucket_idx = 0,
    };
    return it;
}

bool map_iter_next(struct map_iter* it) {
    assert(it != NULL);

    map* m = it->_map;
    for (size_t i = it->_bucket_idx; i < m->n_buckets; i++) {
        struct entry e = m->buckets[i];
        if (e.key == NULL || e.key == tombstone) {
            continue;
        }
        it->key         = e.key;
        it->value       = e.value;
        it->_bucket_idx = i + 1;
        return true;
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
