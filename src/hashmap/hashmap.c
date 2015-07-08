#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "hashmap/hashmap.h"

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
        throw("hashmap init failed");
    } else {
        return map;
    }

error:
    if(map) { map->destroy(map); };
    return NULL;
}

int Hashmap_init(Hashmap *this)
{
    return EXIT_SUCCESS;
}

void Hashmap_destroy(Hashmap * this)
{
    
    if(this) {
        for(int i = 0; i < this->bucket_count; i++){
            this->buckets[i]->destroy(this->buckets[i]);
        }
       	free(this);
    }
}

/**
* void Hashmap_print(Hashmap *this){
*
* PURPOSE : print a hashmap struct
*/
void Hashmap_print(Hashmap *this){
    if(this){

    }
}

/**
* int Hashmap_set(Hashmap *this, const char *key, const void *value, size_t value_size) 
*
* Hashmap           *this; instance to initialize
* const char        *key;
* const void        *value; void pointer to store
* size_t            value_size; 
* 
* PURPOSE :  stores a value in a bucket if it exists, or adds a bucket if it's needed
* RETURN  :  success int
* NOTES   :  lazy for loop being used for now. should be poptimized with a better data
*            structure so lookup time isn't related to the number of keys
*/
int Hashmap_set(Hashmap *this, const char *key, const void *value, size_t value_size) {
    assert(key, "Hashmap :: provide key");
    assert(value, "Hashmap :: provide value");

    assert(this->bucket_count < _MAX_HASHTABLE_KEYS, "Hashmap :: out of space");
    
    uint32_t hash = this->get_hash(key, strlen(key));
    
    int exists = 0;
    int i;

    /* 
        needs to be optimized. considering a trie so lookup time
        is stable at hash length and independent of number of keys
    */
    for(i = 0; i < this->bucket_count; i++){
        if(this->buckets[i]->hash == hash){
            exists = 1;
            break;
        }
    }

    if (exists) {
        this->buckets[i]->set(this->buckets[i], key, value, value_size);
    } else {
        this->buckets[this->bucket_count] = NEW(Bucket, key, value, value_size, hash);
        this->bucket_count++;
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}

/**
* const void * Hashmap_get(Hashmap *this, const char *key) 
*
* Hashmap           *this;
* const char        *key;
* 
* PURPOSE :  retreive a consty value from the hashmap
* RETURN  :  const void *; value stored for the key
* NOTES   :  lazy for loop being used for now. should be poptimized with a better data
*            structure so lookup time isn't related to the number of keys
*/
const void * Hashmap_get(Hashmap *this, const char *key) {
    assert(key, "Hashmap :: provide key");

    uint32_t hash = this->get_hash(key, strlen(key));
    log_info("Hashmap hash :: %u", hash);

    int exists = 0;
    int i = 0;

    /* 
        needs to be optimized. considering a trie so lookup time
        is stable at hash length and independent of number of keys
    */
    for(i = 0; i < this->bucket_count; i++){
        if(this->buckets[i]->hash == hash){
            exists = 1;
            break;
        }
    }

    if(exists){
        return this->buckets[i]->get(this->buckets[i]);
    } else {
        return NULL;
    }

error:
    return NULL;
}

/**
* uint32_t Hashmap_get_hash(const char *key, size_t len)
*
* Hashmap           *this;
* const char        *key;
* size_t            *len; key length
* 
* PURPOSE :  return a simple jenkins hash for a given string 
*            (https://en.wikipedia.org/wiki/Jenkins_hash_function)
* RETURN  :  uint32_t; the hash
*/
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