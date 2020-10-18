#include <stdlib.h>
#include "hashmap.h"
#include <pthread.h>

pthread_mutex_t mylock= PTHREAD_MUTEX_INITIALIZER;

// To ensure we have a hashmap which can be used with any kind of hashing function
// and that we have a comparison function and destruction function for complex objects.
// The hash function will not involve the compression component,
// you will need to compress the hash to fit your implementation.
// If the hash, cmp or destruction function pointers are NULL, this function should return NULL.
struct hash_map* hash_map_new(size_t (*hash)(void*), int (*cmp)(void*,void*),
    void (*key_destruct)(void*), void (*value_destruct)(void*)) {
   if(key_destruct==NULL||value_destruct==NULL||hash==NULL||cmp==NULL){
     return NULL;
   }

  struct hash_map* hash_m = (struct hash_map*)malloc(sizeof(struct hash_map));

   hash_m->hash_func=hash;
   hash_m->cmp_func=cmp;
   hash_m->key_destruct_func=key_destruct;
   hash_m->value_destruct_func=value_destruct;

   hash_m->map_capacity = 16;
   hash_m->item_map=(struct hash_item**)calloc(hash_m->map_capacity, sizeof(struct hash_item*)); // all nullptr
   hash_m->num_hash_item = 0;

   return hash_m;
}

// The put functions will insert a key and value into the hashmap,
// in the event the key exists within the hashmap, the value will be replaced,
// since the data structure owns the data, you will need to free the data.
// It is recommended you record the size variable with the entry.
void hash_map_put_entry_move(struct hash_map* map, void* k, void* v) {
  // check if k exist in the hash map
  // if exist:
  //  replace with the input value
  // else doesn't exist
  //  insert new key
  //  if new_num_of_item > loadfactor * map->capacity
  //    rehashing
  pthread_mutex_lock(&mylock);

  size_t hash_value = map->hash_func(k) % map->map_capacity;

  struct hash_item* item = map->item_map[hash_value], *prev_item = NULL;
  while(item != NULL) {
    // check if k exist
    if(map->cmp_func(item->key,k)==0){
      // replace with the input value
      map->value_destruct_func(item->value);
      item->value=v;
      pthread_mutex_unlock(&mylock);
      return;
    }
    prev_item=item;
    item=item->next;
  }

  // insert new key to the map
  if(map->item_map[hash_value]==NULL){
    map->item_map[hash_value]=(struct hash_item*)malloc(sizeof(struct hash_item));
    map->item_map[hash_value]->key=k;
    map->item_map[hash_value]->value=v;
    map->item_map[hash_value]->next=NULL;
  }else{
    prev_item->next=(struct hash_item*)malloc(sizeof(struct hash_item));
    prev_item->next->key=k;
    prev_item->next->value=v;
    prev_item->next->next=NULL;
  }
  map->num_hash_item=map->num_hash_item+1;

  size_t new_n_item = map->num_hash_item;
  if (new_n_item > 0.75 * map->map_capacity) {
    // rehashing
    size_t new_capacity = map->map_capacity * 2;
    struct hash_item** new_item_map =(struct hash_item**)calloc(new_capacity, sizeof(struct hash_item*));

    for(size_t i = 0; i < map->map_capacity; i++) {

      if (map->item_map[i] != NULL) {

        struct hash_item* curr = map->item_map[i];

        while (curr != NULL) {

          struct hash_item* next = curr->next;

          // insert to new map
          size_t new_hash= map->hash_func(curr->key) % new_capacity;
          //head of index
          struct hash_item* temp_item_list = new_item_map[new_hash];
          new_item_map[new_hash] = curr;
          new_item_map[new_hash]->next = temp_item_list;

          curr = next;
        }
      }
    }

    free(map->item_map);
    map->item_map = new_item_map;
    map->map_capacity = new_capacity;
  }

  pthread_mutex_unlock(&mylock);

}

// If the key exists with the hashmap, it will be removed as well as a the value,
// if the key does not exist, nothing will be removed.
void hash_map_remove_entry(struct hash_map* map, void* k) {
  //check the key exists
  pthread_mutex_lock(&mylock);
  size_t hash_value = map->hash_func(k) % map->map_capacity;

  struct hash_item* item = map->item_map[hash_value];
  struct hash_item* prev_item = map->item_map[hash_value];

  while (item != NULL) {

    // check if k exist
    if(map->cmp_func(item->key,k)==0){
      // delete the key and value
      map->value_destruct_func(item->value);
      map->key_destruct_func(item->key);

      // first item is the one we need to remove
      if(prev_item==item) {
        map->item_map[hash_value] = item->next;
      } else { // not the first item
        prev_item->next = item->next;
      }

      free(item);
      pthread_mutex_unlock(&mylock);
      return;
    }

    prev_item = item;
    item = item->next;
  }
    pthread_mutex_unlock(&mylock);
}
//Your hashmap should retrieve the value given the key.
//In the event the key does not exist, your function should return NULL
void* hash_map_get_value_ref(struct hash_map* map, void* k) {
  if(k==NULL){
    return NULL;
  }
  pthread_mutex_lock(&mylock);

  size_t hash_value = map->hash_func(k) %map->map_capacity;

  struct hash_item* item = map->item_map[hash_value];
  while (item != NULL) {
    // check if k exist
    if(map->cmp_func(item->key,k)==0){
      pthread_mutex_unlock(&mylock);
      return item->value;
    }
    item=item->next;

  }
  pthread_mutex_unlock(&mylock);
  return NULL;
}

void hash_map_destroy(struct hash_map* map) {
  pthread_mutex_lock(&mylock);
  for(size_t i = 0; i < map->map_capacity; i++){

    while(map->item_map[i]!=NULL){
      struct hash_item* next = map->item_map[i]->next;
      map->key_destruct_func(map->item_map[i]->key);
      map->value_destruct_func(map->item_map[i]->value);
      free(map->item_map[i]);
      map->item_map[i]=next;
    }

  }
  free(map->item_map);
  free(map);
  pthread_mutex_unlock(&mylock);
}
