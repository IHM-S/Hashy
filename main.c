#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "hashmap.h"

size_t size_t_hash_func(void* key) {
    return *(size_t*)key;
}

int size_t_key_cmp(void* k1, void* k2) {
    const size_t key1 = *(size_t*) k1;
    const size_t key2 = *(size_t*) k2;
    return key1 == key2 ? 1 : 0;
}

void size_t_key_destructor(void* key) {
}

void size_t_value_destructor(void* value) {
}

void test_1() {
    struct hash_map* m =  hash_map_new(size_t_hash_func, size_t_key_cmp, size_t_key_destructor, size_t_value_destructor);

    size_t k = 1;
    size_t v = 2;

    hash_map_put_entry_move(m, &k, &v);

    size_t* rv = hash_map_get_value_ref(m, &k);

    hash_map_destroy(m);

    assert(*rv == v);
}

int main () {

    printf("Start unittests\n");

    // test_1();

    printf("End unittests\n");
    
    return 0;
}