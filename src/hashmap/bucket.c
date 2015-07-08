#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "hashmap/bucket.h"

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

/**
* int Bucket_init(Bucket *this, const char *key, const void *value, size_t value_size, uint32_t hash)
*
* Bucket            *this; instance to initialize
* const char        *key; unhashed key - will eventualy be used to chain buckets 
*                                        together in case of hash collision 
* const void        *value; void const-y pointer to value to store
* size_t            value_size; size of object to store (since i don't have access to type)
* uint32_t          hash;
* 
* PURPOSE :  initilize the bucket, store the hash and use the set function to copy
*            the kye, value, and hash
* RETURN  :  success bool
* NOTES   :  when objects are placed into a bucket their contents are copied and 
*            ownership is retained by the bucket until it is freed
*
*            anyone who wants to manipulate a value retreived from a bucket should 
*            copy the object and make sure to clean up their manipulated object
*/
int Bucket_init(Bucket *this, const char *key, const void *value, size_t value_size, uint32_t hash){
    this->key = NULL;
    this->value = NULL;
    this->hash = hash;

    return this->set(this, key, value, value_size);
};

void Bucket_destroy(Bucket *this){
    if(this) {
        if(this->key) { free(this->key); };
        if(this->value) { free(this->value); };
        free(this);
    }
};

/**
* void Bucket_print(Bucket *this){
*
* PURPOSE : print a bucket struct
*/
void Bucket_print(Bucket *this){

};

/**
* int Bucket_set(Bucket *this, const char *key, const void *value, size_t value_size)
*
* Bucket            *this; instance to initialize
* const char        *key; unhashed key - will eventualy be used to chain buckets 
*                                        together in case of hash collision 
* const void        *value; void const-y pointer to value to store
* size_t            value_size; size of object to store (since i don't have access to type)
* 
* PURPOSE :  take ownership of a copy of the key, value, and hash of this bucket
* RETURN  :  success bool
* NOTES   :  when objects are placed into a bucket their contents are copied and 
*            ownership is retained by the bucket until it is freed
*
*            anyone who wants to manipulate a value retreived from a bucket should 
*            copy the object and make sure to clean up their manipulated object
*
*/
int Bucket_set(Bucket *this, const char *key, const void *value, size_t value_size){
    if(this->key != NULL) { free(this->key); }
    if(this->value != NULL) { free(this->value); }

    this->key = NULL;
    this->value = NULL;

    this->key = malloc(strlen(key) + 1);
    check_mem(this->key);
    strcpy(this->key, key);

    this->value = calloc(1, value_size + 1);
    check_mem(this->value);
    memcpy(this->value, value, value_size);

    return EXIT_SUCCESS;

error:
   return EXIT_FAILURE;
};

/**
* const void *  Bucket_get(Bucket *this)
*
* Bucket            *this; instance to initialize
* 
* PURPOSE :  return a consty pointer to the value stored in this bucket
* RETURN  :  const void * - a const void pointer to the value stored in this bucker
* NOTES   :  later on buckets with the same hash but different keys will be chained in a 
*         :  linked list
*/
const void * Bucket_get(Bucket *this){
    return this->value;
};