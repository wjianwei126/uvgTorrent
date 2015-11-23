#ifndef _thread_h
#define _thread_h

#include <pthread.h>
#include "macro/macro.h"
#include "utils/thread_pool/job.h"
#include "data_structures/linkedlist/linkedlist.h"

typedef struct Thread Thread;

struct Thread {
    int (*init)(Thread *this, int id, Linkedlist * jobs, pthread_mutex_t * mutex);
    void (*print)(Thread *this);
    void (*destroy)(Thread *this);

    void * (*run)(void *this);
    void (*start)(Thread *this);
   	
	int id;
	int running;
	pthread_t thread;
    pthread_mutex_t * mutex;

    Linkedlist * jobs;
};

Thread *Thread_new(size_t size, int id, Linkedlist * jobs, pthread_mutex_t * mutex);
int Thread_init(Thread *this, int id, Linkedlist * jobs, pthread_mutex_t * mutex);
void Thread_destroy(Thread *this);
void Thread_print(Thread *this);

void Thread_start(Thread *this);
void * Thread_run(void *this);

#endif