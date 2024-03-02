#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

typedef struct map map;

map*   map_new(void);
void*  map_get(const map* m, const char* key);
void*  map_set(map* m, const char* key, void* value);
void   map_del(map* m, const char* key);
size_t map_len(const map* m);
void   map_free(map* m);

struct map_iter {
    char*      key;
    void*      value;
    const map* _map;
    size_t     _bucket_idx;
};

struct map_iter map_iter_new(const map* m);
bool            map_iter_next(struct map_iter* it);

#endif // MAP_H
