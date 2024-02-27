#ifndef MAP_H
#define MAP_H

#include <stddef.h>

typedef struct map map;

map*  map_new(size_t n_buckets);
void* map_get(map* m, const char* key);
void  map_set(map* m, const char* key, void* value);
void  map_del(map* m, const char* key);
void  map_free(map* m);

#endif // MAP_H
