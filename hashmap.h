#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdlib.h>

// typedef struct hash_item hash_item;
// typedef struct hash_map hash_map;

struct hash_item {
    void* key;
    void* value;

    struct hash_item* next;
};

struct hash_map {
    size_t (*hash_func)(void*);
    int (*cmp_func)(void*, void*);
    void (*key_destruct_func)(void*);
    void (*value_destruct_func)(void*);

    struct hash_item** item_map;
    size_t map_capacity;
    size_t num_hash_item;
};

/*
 * Create a new hash map
 * @param hash, the hash function with calculate hash value the key
 * @param cmp, the comparasion function which used to compare the map keys
 * @param key_destruct, the function which used by map when it needs to destroy
 * the key
 * @param value_destruct, the function which used by map when it needs to
 * destroy the value
 * @return new hash map, if the hash, cmp or destruction function pointers are
 * NULL, this function will return NULL.
 */
struct hash_map* hash_map_new(size_t (*hash)(void*),
                              int (*cmp)(void*, void*),
                              void (*key_destruct)(void*),
                              void (*value_destruct)(void*));

/*
 * Put a new entry into the hash map
 * @param k, the key of the new pair
 * @param v, the value of the new pair
 */
void hash_map_put_entry_move(struct hash_map* map, void* k, void* v);

/*
 * Remove from the map with input key
 * @param k, the key in the map to remove
 */
void hash_map_remove_entry(struct hash_map* map, void* k);

/*
 * Get the value for the input key in the map
 * @param k, the key we want to get
 */
void* hash_map_get_value_ref(struct hash_map* map, void* k);

/*
 * the hash map object desctructor
 */
void hash_map_destroy(struct hash_map* map);

#endif
