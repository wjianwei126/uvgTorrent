#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "dict/dictionary.h"

void Dictionary_destroy(void * self)
{
    Dictionary * this = self;
    
    if(this) {
        for(int i = 0; i < this->bucket_count; i++){
            this->buckets[i]->destroy(this->buckets[i]);
        }
       	free(this);
    }
}

int Dictionary_init(void *self)
{
    return EXIT_SUCCESS;
}

void *Dictionary_new(size_t size)
{
    Dictionary *dict = malloc(size);
    check_mem(dict);

    dict->init = Dictionary_init;
    dict->destroy = Dictionary_destroy;
    dict->print = Dictionary_print;

    dict->get = Dictionary_get;
    dict->set = Dictionary_set;
    dict->get_hash = Dictionary_get_hash;

    dict->bucket_count = 0;

    if(dict->init(dict) == EXIT_FAILURE) {
        throw("dictionary init failed");
    } else {
        // all done, we made an object of any type
        return dict;
    }

    error:
    	if(dict) { dict->destroy(dict); };
        return NULL;
}

void Dictionary_print(void *self){
    Dictionary * this = self;
    if(this){

    }
}

/* get / set */
int Dictionary_set(void *self, const char *key, const void *value, size_t value_size) {
    Dictionary * this = self;

    assert(key, "Dictionary :: provide key");
    assert(value, "Dictionary :: provide value");

    assert(this->bucket_count < _MAX_DICTIONARY_KEYS, "Dictionary :: out of space");
    
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

void * Dictionary_get(void *self, const char *key) {
    assert(key, "Dictionary :: provide key");

    Dictionary * this = self;

    uint32_t hash = this->get_hash(key, strlen(key));
    log_info("dictionary hash :: %u", hash);

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

uint32_t Dictionary_get_hash(const char *key, size_t len) {
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




/* constructor / destructor functions */
void *Bucket_new(size_t size, const char *key, const void *value, size_t value_size, uint32_t hash){
    Bucket *bucket = malloc(size);
    check_mem(bucket);

    bucket->init = Bucket_init;
    bucket->destroy = Bucket_destroy;
    bucket->print = Bucket_print;

    bucket->get = Bucket_get;
    bucket->set = Bucket_set;
    bucket->get_hash = Dictionary_get_hash;

    if(bucket->init(bucket, key, value, value_size, hash) == EXIT_FAILURE) {
        throw("bucket init failed");
    } else {
        // all done, we made an object of any type
        return bucket;
    }

    error:
        if(bucket) { bucket->destroy(bucket); };
        return NULL;
};

int Bucket_init(void *self, const char *key, const void *value, size_t value_size, uint32_t hash){
    Bucket * this = self;

    this->key = NULL;
    this->value = NULL;
    this->hash = 0;

    this->key = malloc(strlen(key) + 1);
    check_mem(this->key);
    strcpy(this->key, key);

    this->value = calloc(1, value_size + 1);
    check_mem(this->value);
    memcpy(this->value, value, value_size);

    this->hash = hash;

    return EXIT_SUCCESS;

error:
   return EXIT_FAILURE;
};

void Bucket_print(void *self){

};

void Bucket_destroy(void *self){
    Bucket * this = self;
    
    if(this) {
        if(this->key) { free(this->key); };
        if(this->value) { free(this->value); };
        free(this);
    }
};

/* get / set */
int Bucket_set(void *self, const char *key, void *value, size_t value_sizem){

    return 1;
};

void * Bucket_get(void *self, const char *key){

    return self;
};