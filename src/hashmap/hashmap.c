#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "hashmap/hashmap.h"

void Hashmap_destroy(Hashmap * this)
{
    
    if(this) {
        for(int i = 0; i < this->bucket_count; i++){
            this->buckets[i]->destroy(this->buckets[i]);
        }
       	free(this);
    }
}

int Hashmap_init(Hashmap *this)
{
    return EXIT_SUCCESS;
}

Hashmap *Hashmap_new(size_t size)
{
    Hashmap *map = malloc(size);
    check_mem(map);

    map->init = Hashmap_init;
    map->destroy = Hashmap_destroy;
    map->print = Hashmap_print;

    map->get = Hashmap_get;
    map->set = Hashmap_set;
    map->get_hash = Hashmap_get_hash;

    map->bucket_count = 0;

    if(map->init(map) == EXIT_FAILURE) {
        throw("dictionary init failed");
    } else {
        // all done, we made an object of any type
        return map;
    }

    error:
    	if(map) { map->destroy(map); };
        return NULL;
}

void Hashmap_print(Hashmap *this){
    if(this){

    }
}

/* get / set */
int Hashmap_set(Hashmap *this, const char *key, const void *value, size_t value_size) {
    assert(key, "Hashmap :: provide key");
    assert(value, "Hashmap :: provide value");

    assert(this->bucket_count < _MAX_HASHTABLE_KEYS, "Hashmap :: out of space");
    
    uint32_t hash = this->get_hash(key, strlen(key));
    
    int exists = 0;
    int i;

    /* OPTIMIZE : Binary Search */
    for(i = 0; i < this->bucket_count; i++){
        if(this->buckets[i]->hash == hash){
            exists = 1;
            break;
        }
    }

    if (exists) {
        if(this->buckets[i]->key) { free(this->buckets[i]->key); };
        if(this->buckets[i]->value) { free(this->buckets[i]->value); };
        this->buckets[i]->init(this->buckets[i], key, value, value_size, hash);
    } else {
        this->buckets[this->bucket_count] = NEW(Bucket, key, value, value_size, hash);
        this->bucket_count++;
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}

void * Hashmap_get(Hashmap *this, const char *key) {
    assert(key, "Hashmap :: provide key");

    uint32_t hash = this->get_hash(key, strlen(key));
    log_info("Hashmap hash :: %u", hash);

    int exists = 0;
    int i = 0;
    for(i = 0; i < this->bucket_count; i++){
        if(this->buckets[i]->hash == hash){
            exists = 1;
            break;
        }
    }

    if(exists){
        return this->buckets[i]->value;
    } else {
        return NULL;
    }

error:
    return NULL;
}

uint32_t Hashmap_get_hash(const char *key, size_t len) {
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}