#include "macro/macro.h"

#ifndef _linkednode_h
#define _linkednode_h

/* Linkednode */
typedef struct Linkednode Linkednode;
struct Linkednode {
	int (*init)(Linkednode *this, const void * value, size_t value_size);
    void (*print)(Linkednode *this);
    void (*destroy)(Linkednode *this);

    int (*set)(Linkednode *this, const void * value, size_t value_size);
    const void * (*get)(Linkednode *this);

    Linkednode *next;
    void *value;
    size_t size;
};

/* basic functions */
Linkednode * Linkednode_new(size_t size, const void * value, size_t value_size);
int Linkednode_init(Linkednode *this, const void * value, size_t value_size);
void Linkednode_destroy(Linkednode *this);
void Linkednode_print(Linkednode *this);

int Linkednode_set(Linkednode *this, const void * value, size_t value_size);
const void * Linkednode_get(Linkednode *this);
#endif