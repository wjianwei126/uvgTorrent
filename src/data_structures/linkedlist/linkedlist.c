#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "data_structures/linkedlist/linkedlist.h"

Linkedlist * Linkedlist_new(size_t size)
{
    Linkedlist *list = malloc(size);
    check_mem(list);

    list->init = Linkedlist_init;
    list->destroy = Linkedlist_destroy;
    list->print =  Linkedlist_print;

    list->get = Linkedlist_get;
    list->append = Linkedlist_append;

    if(list->init(list) == EXIT_FAILURE) {
        throw("linkedlist init failed");
    } else {
        return list;
    }

error:
    if(list) { list->destroy(list); };
    return NULL;
}

/**
* int Linkedlist_init(Linkednode *this)
*
* Linkedlist        *this; instance to initialize
* 
* PURPOSE :  initilize the list & NULL out properties
* RETURN  :  success bool
* NOTES   :  headless list will have head == NULL and count == -1
*/
int Linkedlist_init(Linkedlist *this)
{
    this->head = NULL;
    this->end = NULL;
    this->count = -1;

    return EXIT_SUCCESS;
}

/**
* void Linkedlist_destroy(Linkedlist *this)
*
* Linkedlist        *this; instance to destory
* 
* PURPOSE :  destroy the list
* NOTES   :  headless list will have head == NULL and count == -1
*            when the first node is destroyed it will tell it's next node to destory as well.
*/
void Linkedlist_destroy(Linkedlist *this)
{
    if(this){
        if(this->head){
            this->head->destroy(this->head);
        }
        free(this);
    }
}

void Linkedlist_print(Linkedlist *this)
{

}

/**
* const void * Linkedlist_get(Linkedlist *this, int pos)
*
* Linkedlist        *this; 
* int               pos; position of the value to retreive
*
* PURPOSE :  retrieve a consty value
* RETURN  :  cont void pointer to value
*/
const void * Linkedlist_get(Linkedlist *this, int pos)
{   
    if(this->head == NULL) { return NULL; }
    if(pos > this->count) { throw("linkedlist :: index %d out of bounds", pos); }

    Linkednode *curr = this->head;
    for(int i = 0; i < pos; i++){
        curr = curr->next;
    }

    return curr->get(curr);

error:
    return NULL;
}

/**
* int Linkedlist_set(Linkedlist *this, int pos, const void *value, size_t value_size)
*
* Linkedlist        *this;
* int               pos; position of the value to retreive
* const void        *value; value to save
* size_t            value_size;
*
* PURPOSE :  save a value into a given pre-existing position
* RETURN  :  success int
*/
int Linkedlist_set(Linkedlist *this, int pos, const void *value, size_t value_size){
    if(this->head == NULL) { return EXIT_FAILURE; }
    if(pos > this->count) { throw("linkedlist :: index %d out of bounds", pos); }

    Linkednode *curr = this->head;
    for(int i = 0; i < pos; i++){
        curr = curr->next;
    }

    return curr->set(curr, value, value_size);

error:
    return EXIT_FAILURE;
}

/**
* int Linkedlist_append(Linkedlist *this, const void *value, size_t value_size)
*
* Linkedlist        *this;
* const void        *value; value to save
* size_t            value_size;
*
* PURPOSE :  append a value to the end of the linked list
* RETURN  :  success int
*/
int Linkedlist_append(Linkedlist *this, const void *value, size_t value_size){
    Linkednode * node = NEW(Linkednode, value, value_size);
    check_mem(node);

    if(this->head == NULL){
        this->head = node;
        this->end = node;
    }

    this->end->next = node;
    this->end = node;


    this->count++;


    return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}   
