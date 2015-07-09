#include "macro/macro.h"
#include "data_structures/hashmap/bucket.h"
#include <stdint.h>

#ifndef _hashtable_h
#define _hashtable_h

#define _MAX_HASHTABLE_KEYS 100

/* Hashmap */
typedef struct Hashmap Hashmap;
struct Hashmap {
	int (*init)(Hashmap *this);
    void (*print)(Hashmap *this);
    void (*destroy)(Hashmap *this);

    int bucket_count;
    Bucket * buckets[_MAX_HASHTABLE_KEYS];
	uint32_t (*get_hash)(char const *key, size_t len);
    const void * (*get)(Hashmap *this, const char *key);
    int (*set)(Hashmap *this, const char *key, const void *value, size_t value_size);
};

/* basic functions */
Hashmap * Hashmap_new(size_t size);
int Hashmap_init(Hashmap *this);
void Hashmap_destroy(Hashmap *this);
void Hashmap_print(Hashmap *this);

const void * Hashmap_get(Hashmap *this, const char *key);
int Hashmap_set(Hashmap *this, const char *key, const void *value, size_t value_size);
uint32_t Hashmap_get_hash(const char *key, size_t len);

#endif