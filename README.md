# libds

Data structures implemented in C.

## Map

A hash map with support for dynamic resizing and custom allocators.

```c
map* m = map_new(NULL);

map_get(m, "foo"); // -> NULL
map_len(m);        // -> 0

map_set(m, "foo", "bar"); // -> "bar"
map_get(m, "foo");        // -> "bar"
map_len(m);               // -> 1

map_set(m, "foo", "baz"); // -> "bar"
map_get(m, "foo");        // -> "baz"
map_len(m);               // -> 1

map_del(m, "foo"); // -> "baz"
map_len(m);        // -> 0

for (struct map_iter it = map_iter_new(m); map_iter_next(&it);) {
    // it.key
    // it.value
}

map_free(m);
```

There are two implementations: [open addressing][1] ([map_oa.c](map_oa.c)) and [separate chaining][2] ([map_sc.c](map_sc.c)).
Both are based on information from Wikipedia and the book [Crafting interpreters][3].
The hash function used is [FNV-1a][4].

[1]: https://en.wikipedia.org/wiki/Hash_table#Open_addressing
[2]: https://en.wikipedia.org/wiki/Hash_table#Separate_chaining
[3]: https://craftinginterpreters.com/hash-tables.html
[4]: https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function#FNV-1a_hash
