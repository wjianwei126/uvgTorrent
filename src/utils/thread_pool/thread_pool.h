#ifndef _thread_pool_h
#define _thread_pool_h

#include <pthread.h>
#include "macro/macro.h"
#include "utils/thread_pool/thread.h"
#include "data_structures/linkedlist/linkedlist.h"

typedef struct Thread_Pool Thread_Pool;

struct Thread_Pool {
    int (*init)(Thread_Pool *this);
    void (*print)(Thread_Pool *this);
    void (*destroy)(Thread_Pool *this);
   	
    Linkedlist * threads;
    Linkedlist * jobs;

    const int max_threads;
    volatile int thread_count;
	volatile int working_threads;
	pthread_mutex_t thread_count_lock;
};

Thread_Pool *Thread_Pool_new(size_t size, const int max_threads);
int Thread_Pool_init(Thread_Pool *this, const int max_threads);
void Thread_Pool_destroy(Thread_Pool *this);
void Thread_Pool_print(Thread_Pool *this);
#endif