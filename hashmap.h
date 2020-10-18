#ifndef HASHMAP_H
#define HASHMAP_H


#include <stdlib.h>

//typedef struct hash_item hash_item;
//typedef struct hash_map hash_map;

struct hash_item{
  void *key;
  void *value;

  struct hash_item *next;
};

struct hash_map{

  size_t (*hash_func)(void*);
  int (*cmp_func)(void*,void*);
  void (*key_destruct_func)(void*);
  void (*value_destruct_func)(void*);

  struct hash_item** item_map;
  size_t map_capacity;
  size_t num_hash_item;
}; //Modify this!

struct hash_map* hash_map_new(size_t (*hash)(void*), int (*cmp)(void*,void*),
    void (*key_destruct)(void*), void (*value_destruct)(void*));

void hash_map_put_entry_move(struct hash_map* map, void* k, void* v);

void hash_map_remove_entry(struct hash_map* map, void* k);

void* hash_map_get_value_ref(struct hash_map* map, void* k);

void hash_map_destroy(struct hash_map* map);

#endif
