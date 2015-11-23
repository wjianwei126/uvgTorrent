#ifndef _thread_pool_h
#define _thread_pool_h

#include <pthread.h>
#include "macro/macro.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "utils/thread_pool/job.h"
#include "utils/thread_pool/thread.h"

typedef struct Thread_Pool Thread_Pool;
struct Thread_Pool {
    int (*init)(Thread_Pool *this, const int max_threads);
    void (*print)(Thread_Pool *this);
    void (*destroy)(Thread_Pool *this);

    void (*add_job)(Thread_Pool *this, Job *job);
    Thread * (*get_available_thread)(Thread_Pool *this);
    void (*process_next_job)(Thread_Pool *this);
   	
    Linkedlist * threads;
    pthread_mutex_t jobs_mutex;
    Linkedlist * jobs;

    int max_threads;
    volatile int thread_count;
    volatile int working_threads;
    volatile int current_job;
};

Thread_Pool *Thread_Pool_new(size_t size, const int max_threads);
int Thread_Pool_init(Thread_Pool *this, const int max_threads);
void Thread_Pool_destroy(Thread_Pool *this);
void Thread_Pool_print(Thread_Pool *this);

void Thread_Pool_add_job(Thread_Pool *this, Job *job);
#endif