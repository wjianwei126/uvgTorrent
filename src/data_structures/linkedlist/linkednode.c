#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "data_structures/linkedlist/linkednode.h"

Linkednode * Linkednode_new(size_t size, const void * value, size_t value_size)
{
	Linkednode *node = calloc(1, size);
    check_mem(node);

    node->init = Linkednode_init;
    node->destroy = Linkednode_destroy;
    node->print =  Linkednode_print;

    node->set = Linkednode_set;
    node->get = Linkednode_get;

    if(node->init(node, value, value_size) == EXIT_FAILURE) {
        throw("linkednode init failed");
    } else {
        return node;
    }

error:
    if(node) { node->destroy(node); };
    return NULL;
}

/**
* int Linkednode_init(Linkednode *this, const void * value, size_t value_size)
*
* Linkednode        *this; instance to initialize
* const void        *value; void const-y pointer to value to store
* size_t            value_size; size of object to store
* 
* PURPOSE :  initilize the node, NULL out properties and use the set value to store a copy of the pointer being saved
* RETURN  :  success bool
* NOTES   :  when a pointer is placed into a node it is copied and 
*            ownership is retained by the node until it is freed
*
*            anyone who wants to manipulate a value retreived from a node should 
*            copy the object and make sure to clean up their manipulated object
*/
int Linkednode_init(Linkednode *this, const void * value, size_t value_size)
{
	this->value = NULL;
	this->size = 0;

	return this->set(this, value, value_size);
}

void Linkednode_destroy(Linkednode *this){
	if(this){
    	if(this->value) { 
            free(this->value);
        }
		if(this->next) { this->next->destroy(this->next); }
		free(this);
	}
}

void Linkednode_print(Linkednode *this)
{

}

/**
* int Linkednode_set(Linkednode *this, const void * value, size_t value_size)
*
* Linkednode        *this; instance to initialize
* const void        *value; void const-y pointer to value to store
* size_t            value_size; size of object to store
* 
* PURPOSE :  store a copy of the value pointer
* RETURN  :  success bool
* NOTES   :  when a pointer is placed into a node it is copied and 
*            ownership is retained by the node until it is freed
*
*            anyone who wants to manipulate a value retreived from a node should 
*            copy the object and make sure to clean up their manipulated object
*/
int Linkednode_set(Linkednode *this, const void * value, size_t value_size)
{
    if(this->value != NULL) { free(this->value); }

    this->value = NULL;
    this->size = value_size;

    this->value = calloc(1, this->size + 1);
    check_mem(this->value);
    memcpy(this->value, value, this->size);

    return EXIT_SUCCESS;

error:
   return EXIT_FAILURE;

}

/**
* const void * Linkednode_get(Linkednode *this)
*
* Linkednode        *this; instance to initialize
* 
* PURPOSE :  return the value pointer
* RETURN  :  consty pointer to the value
* NOTES   :  when a pointer is placed into a node it is copied and 
*            ownership is retained by the node until it is freed
*
*            anyone who wants to manipulate a value retreived from a node should 
*            copy the object and make sure to clean up their manipulated object
*/
const void * Linkednode_get(Linkednode *this)
{
	return this->value;
}