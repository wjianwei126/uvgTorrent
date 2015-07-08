#include "macro/macro.h"
#include <stdint.h>

#ifndef _bucket_h
#define _bucket_h

/* BUCKET */
typedef struct Bucket Bucket;
struct Bucket
{
	int (*init)(Bucket *this, const char *key, const void *value, size_t value_size, uint32_t hash);
    void (*print)(Bucket *this);
    void (*destroy)(Bucket *this);

	const void * (*get)(Bucket *this);
	int (*set)(Bucket *this, const char *key, const void *value, size_t value_size);

	char * key;
	uint32_t hash;
	void * value;
	Bucket * next;
};

/* basic functions */
Bucket *Bucket_new(size_t size, const char *key, const void *value, size_t value_size, uint32_t hash);
int Bucket_init(Bucket *this, const char *key, const void *value, size_t value_size, uint32_t hash);
void Bucket_print(Bucket *this);
void Bucket_destroy(Bucket *this);

int Bucket_set(Bucket *this, const char *key, const void *value, size_t value_size);
const void * Bucket_get(Bucket *this);
#endif