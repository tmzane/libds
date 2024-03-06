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
#include <stdlib.h>
#include <string.h>

const size_t INIT_N_BUCKETS = 8;

size_t max_n_entries(size_t n_buckets) {
    return n_buckets / 4 * 3;
}

static uint64_t hash(const char* key);
static map*     map_resize(map* m, size_t n_buckets);

struct entry {
    char* key;
    void* value;
};

struct map {
    size_t        n_entries;
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
        if (strcmp(e.key, key) == 0) {
            return e.value;
        }
    }
}

void* map_set(map* m, const char* key, const void* value) {
    assert(m != NULL);
    assert(key != NULL);
    assert(value != NULL);

    for (size_t i = hash(key) % m->n_buckets;; i = (i + 1) % m->n_buckets) {
        struct entry* e = &m->buckets[i];
        if (e->key == NULL) {
            e->key   = strdup(key);
            e->value = (void*)value;
            m->n_entries++;
            break;
        }
        if (strcmp(e->key, key) == 0) {
            void* prev = e->value;
            e->value   = (void*)value;
            return prev;
        }
    }

    if (m->n_entries > max_n_entries(m->n_buckets)) {
        if (map_resize(m, m->n_buckets * 2) == NULL) {
            return NULL;
        }
    }

    return (void*)value;
}

// TODO: implement
void map_del(map* m, const char* key) {}

size_t map_len(const map* m) {
    assert(m != NULL);
    return m->n_entries;
}

void map_free(map* m) {
    assert(m != NULL);
    for (size_t i = 0; i < m->n_buckets; i++) {
        free(m->buckets[i].key);
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
        if (e.key == NULL) {
            continue;
        }
        for (size_t j = hash(e.key) % n_buckets;; j = (j + 1) % n_buckets) {
            if (new_buckets[j].key != NULL) {
                continue;
            }
            new_buckets[j] = e;
            break;
        }
    }

    free(m->buckets);
    m->n_buckets = n_buckets;
    m->buckets   = new_buckets;

    return m;
}

// TODO: implement
void map_print(const map* m, void print_value(void*)) {}

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
    for (; it->_bucket_idx < m->n_buckets; it->_bucket_idx++) {
        struct entry e = m->buckets[it->_bucket_idx];
        if (e.key == NULL) {
            continue;
        }
        it->key   = e.key;
        it->value = e.value;
        it->_bucket_idx++;
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
