#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "hashmap/bucket.h"

/* constructor / destructor functions */
Bucket *Bucket_new(size_t size, const char *key, const void *value, size_t value_size, uint32_t hash){
    Bucket *bucket = malloc(size);
    check_mem(bucket);

    bucket->init = Bucket_init;
    bucket->destroy = Bucket_destroy;
    bucket->print = Bucket_print;

    bucket->get = Bucket_get;
    bucket->set = Bucket_set;

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

int Bucket_init(Bucket *this, const char *key, const void *value, size_t value_size, uint32_t hash){
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

void Bucket_print(Bucket *this){

};

void Bucket_destroy(Bucket *this){
    if(this) {
        if(this->key) { free(this->key); };
        if(this->value) { free(this->value); };
        free(this);
    }
};

/* get / set */
int Bucket_set(Bucket *this, const char *key, void *value, size_t value_sizem){

    return 1;
};

const void * Bucket_get(Bucket *this, const char *key){
    return this->value;
};