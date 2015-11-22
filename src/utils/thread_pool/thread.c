#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "utils/thread_pool/thread.h"
#include "data_structures/linkedlist/linkedlist.h"

Thread *Thread_new(size_t size, int id, Linkedlist * jobs)
{
	Thread *thread = malloc(size);
    check_mem(thread);

    thread->init = Thread_init;
    thread->destroy = Thread_destroy;
    thread->print= Thread_print;

    thread->run= Thread_run;
    thread->start= Thread_start;

    if(thread->init(thread, id, jobs) == EXIT_FAILURE) {
        throw("thread init failed");
    } else {
        return thread;
    }

error:
    if(thread) { thread->destroy(thread); };
    return NULL;
}

int Thread_init(Thread *this, int id, Linkedlist * jobs)
{
	this->id = id;
    this->running = 1;

    this->jobs = jobs;

    return EXIT_SUCCESS;
}

void Thread_destroy(Thread *this)
{
	if(this){
        free(this);
	}
}

void Thread_start(Thread *this)
{
    debug("START THREAD");
    pthread_create(&this->thread, NULL, this->run, this);
}

void * Thread_run(void *this)
{
    Thread * thread = (Thread *) this;
    while(1){
        Job * job = (Job *) thread->jobs->get(thread->jobs, 0);
        if(job){
            job->print(job);
        }
    }

    return (void *) NULL;
}

void Thread_print(Thread *this)
{
	debug("Thread :: %d", this->id);
}