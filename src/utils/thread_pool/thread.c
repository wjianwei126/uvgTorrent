#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils/thread_pool/thread.h"

Thread *Thread_new(size_t size, int id, Thread_Pool *pool)
{
	Thread *thread = malloc(size);
    check_mem(thread);

    thread->init = Thread_init;
    thread->destroy = Thread_destroy;
    thread->print= Thread_print;

    if(thread->init(thread, id, pool) == EXIT_FAILURE) {
        throw("thread init failed");
    } else {
        return thread;
    }

error:
    if(thread) { thread->destroy(thread); };
    return NULL;
}

int Thread_init(Thread *this, int id, Thread_Pool *pool)
{
	this->id = malloc(sizeof(int));
    check_mem(this->id);
    memcpy(this->id, (void *)&id, sizeof(int));

    this->pool = pool;
}

void Thread_destroy(Thread *this)
{
	if(this){
		if(this->id){ free(this->id); };
        free(this);
	}
}

void Thread_print(Thread *this)
{

}