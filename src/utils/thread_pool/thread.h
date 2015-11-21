#ifndef _thread_h
#define _thread_h

#include "macro/macro.h"
#include "utils/thread_pool/thread_pool.h"

typedef struct Thread Thread;

struct Thread {
    int (*init)(Thread *this, int id, Thread_Pool *pool);
    void (*print)(Thread *this);
    void (*destroy)(Thread *this);
   	
   	Thread_Pool * pool;
	int id;                          /* function's argument       */
};

Thread *Thread_new(size_t size, int id, Thread_Pool *pool);
int Thread_init(Thread *this, int id, Thread_Pool *pool);
void Thread_destroy(Thread *this);
void Thread_print(Thread *this);

#endif