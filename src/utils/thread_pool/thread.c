#include "macro/macro.h"
#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "utils/thread_pool/thread.h"
#include "data_structures/linkedlist/linkedlist.h"

Thread *Thread_new(size_t size, int id, Linkedlist * jobs, pthread_mutex_t * mutex)
{
	Thread *thread = malloc(size);
    check_mem(thread);

    thread->init = Thread_init;
    thread->destroy = Thread_destroy;
    thread->print= Thread_print;

    thread->run= Thread_run;
    thread->start= Thread_start;

    if(thread->init(thread, id, jobs, mutex) == EXIT_FAILURE) {
        throw("thread init failed");
    } else {
        return thread;
    }

error:
    if(thread) { thread->destroy(thread); };
    return NULL;
}

int Thread_init(Thread *this, int id, Linkedlist * jobs, pthread_mutex_t * mutex)
{
	this->id = id;
    this->running = 1;  

    this->thread = 0;
    this->jobs = jobs;
    this->mutex = mutex;

    return EXIT_SUCCESS;
}

void Thread_destroy(Thread *this)
{
	if(this){
        if(this->thread) {
            // use the running bool to tell the thread to exit
            // at the next available opportunity.
            this->running = 0;

            // wait until thread has exited naturaly
            while (pthread_kill(this->thread,0) == 0) { }

            // close down the thread we spawned
            pthread_join(this->thread, NULL);
        };
        free(this);
	}
}

void Thread_start(Thread *this)
{
    pthread_create(&this->thread, NULL, this->run, this);
    pthread_detach( this->thread );
}

void * Thread_run(void *this)
{
    Thread * thread = (Thread *) this;
    while(thread->running == 1){
        Linkednode * node = NULL;
        
        pthread_mutex_lock(thread->mutex);
            node = thread->jobs->pop(thread->jobs);
        pthread_mutex_unlock(thread->mutex);

       #include <signal.h>
        if(node){
            Job * job = (Job *) node->get(node);
            job->function(job->arg);
            job->destroy(job);
            job = NULL;
            
            node->value = NULL;
            node->destroy(node);
        }
    }

    return (void *) NULL;
}

void Thread_print(Thread *this)
{
	debug("Thread :: %d", this->id);
}