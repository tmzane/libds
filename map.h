#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>

/*
 * A hash map with support for dynamic resizing.
 * The keys are strings, the values must not be NULL.
 * Must be created with map_new() and freed with map_free().
 *
 * The actual implementation depends on the compiled source file:
 * map_oa.c implements open addressing, map_sc.c implements separate chaining.
 */
typedef struct map map;

/*
 * Allocates and initializes a new map.
 * Returns a pointer to the map or NULL if out of memory.
 */
map* map_new(void);

/*
 * Finds the value for the given key in the map.
 * Returns the value found or NULL if the key does not exist.
 */
void* map_get(const map* m, const char* key);

/*
 * Sets a new value for the given key in the map.
 * The key is copied, the value is stored as given.
 * If the key already exists, the previous value is overwritten.
 * Returns the given value if the key does not exist in the map.
 * Otherwise, returns the previous value so that it can be freed.
 * Automatically resizes the map if there are too many key/value pairs.
 * If runs out of memory while resizing, returns NULL.
 */
void* map_set(map* m, const char* key, const void* value);

/*
 * Deletes the key/value pair from the map.
 * Returns the value so that it can be freed or NULL if the key does not exist.
 */
void* map_del(map* m, const char* key);

/*
 * Returns the number of key/value pairs in the map.
 */
size_t map_len(const map* m);

/*
 * Frees the memory allocated for the map.
 * The values stored in the map must be freed by the caller.
 */
void map_free(map* m);

/*
 * An iterator for a map.
 * Must be created with map_iter_new().
 * The fields key and value can only be used after map_iter_next() returns true.
 * The fields with names prefixed with underscore are internal and must not be used.
 * Do not call map_set() while iterating, as it may trigger map resizing.
 */
struct map_iter {
    char*  key;
    void*  value;
    map*   _map;
    size_t _bucket_idx;
};

/*
 * Returns a new map iterator.
 */
struct map_iter map_iter_new(const map* m);

/*
 * Moves the iterator to the next key/value pair in the map.
 * Returns true if there are more pairs to iterate over, false otherwise.
 */
bool map_iter_next(struct map_iter* it);

#endif // MAP_H
