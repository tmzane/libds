// A hash map implementation that uses separate chaining [1] to resolve collisions.
//
// [1]: https://en.wikipedia.org/wiki/Hash_table#Separate_chaining

#include "map.h"

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
    size_t        n_buckets;
    struct entry* buckets[];
};

map* map_new(size_t n_buckets) {
    map* m       = malloc(sizeof(map) + n_buckets * sizeof(struct entry*));
    m->n_buckets = n_buckets;
    memset(m->buckets, 0, n_buckets * sizeof(struct entry*));
    return m;
}

void* map_get(map* m, const char* key) {
    uint64_t i = hash(key) % m->n_buckets;

    for (struct entry* e = m->buckets[i]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return e->value;
        }
    }

    return NULL;
}

void map_set(map* m, const char* key, void* value) {
    uint64_t i = hash(key) % m->n_buckets;

    for (struct entry* e = m->buckets[i]; e != NULL; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            e->value = value;
            return;
        }
    }

    struct entry* e = malloc(sizeof(struct entry));
    e->key          = strdup(key);
    e->value        = value;
    e->next         = m->buckets[i];
    m->buckets[i]   = e;
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
    }
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
