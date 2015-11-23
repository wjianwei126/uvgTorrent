#include "macro/macro.h"
#include "data_structures/linkedlist/linkednode.h"

#ifndef _linkedlist_h
#define _linkedlist_h

/* Linkedlist */
typedef struct Linkedlist Linkedlist;
struct Linkedlist {
	int (*init)(Linkedlist *this);
    void (*print)(Linkedlist *this);
    void (*destroy)(Linkedlist *this);

    const void * (*get)(Linkedlist *this, int pos);
    Linkednode * (*pop)(Linkedlist *this);
    int (*set)(Linkedlist *this, int pos, const void *value, size_t value_size);
    int (*append)(Linkedlist *this, const void *value, size_t value_size);

    Linkednode *head;
    Linkednode *end;
    int count;
};

/* basic functions */
Linkedlist * Linkedlist_new(size_t size);
int Linkedlist_init(Linkedlist *this);
void Linkedlist_destroy(Linkedlist *this);
void Linkedlist_print(Linkedlist *this);

const void * Linkedlist_get(Linkedlist *this, int pos);
Linkednode * Linkedlist_pop(Linkedlist *this);
int Linkedlist_set(Linkedlist *this, int pos, const void *value, size_t value_size);
int Linkedlist_append(Linkedlist *this, const void *value, size_t value_size);
#endif