#include "macro/macro.h"
#include <stdint.h>

#ifndef _dictionary_h
#define _dictionary_h

/* BUCKET */
typedef struct
{
	int (*init)(void *self, const char *key, const void *value, size_t value_size, uint32_t hash);
    void (*print)(void *self);
    void (*destroy)(void *self);

	char * key;
	uint32_t hash;
	uint32_t (*get_hash)(const char *key, size_t len);
	void * (*get)(void *self, const char *key);
	int (*set)(void *self, const char *key, void *value, size_t value_size);
	void * value;
} Bucket;

/* constructor / destructor functions */
void *Bucket_new(size_t size, const char *key, const void *value, size_t value_size, uint32_t hash);
int Bucket_init(void *self, const char *key, const void *value, size_t value_size, uint32_t hash);
void Bucket_print(void *self);
void Bucket_destroy(void *self);

/* get / set */
int Bucket_set(void *self, const char *key, void *value, size_t value_size);
void * Bucket_get(void *self, const char *key);

#define _MAX_DICTIONARY_KEYS 100

/* DICTIONARY */
typedef struct {
	int (*init)(void *self);
    void (*print)(void *self);
    void (*destroy)(void *self);

    int bucket_count;
    Bucket * buckets[_MAX_DICTIONARY_KEYS];
	uint32_t (*get_hash)(char const *key, size_t len);
    void * (*get)(void *self, const char *key);
    int (*set)(void *self, const char *key, const void *value, size_t value_size);
} Dictionary;

/* constructor / destructor functions */
void * Dictionary_new(size_t size);
int Dictionary_init(void *self);
void Dictionary_print(void *self);
void Dictionary_destroy(void *self);

void * Dictionary_get(void *self, const char *key);
int Dictionary_set(void *self, const char *key, const void *value, size_t value_size);

uint32_t Dictionary_get_hash(const char *key, size_t len);
#endif