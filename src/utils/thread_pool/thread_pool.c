#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_structures/linkedlist/linkedlist.h"
#include "utils/thread_pool/thread_pool.h"
#include "utils/thread_pool/job.h"
#include "utils/thread_pool/thread.h"

Thread_Pool *Thread_Pool_new(size_t size, const int max_threads)
{
	Thread_Pool *pool = malloc(size);
    check_mem(pool);

    pool->init = Thread_Pool_init;
    pool->destroy = Thread_Pool_destroy;
    pool->print= Thread_Pool_print;

    pool->add_job= Thread_Pool_add_job;

    if(pool->init(pool, max_threads) == EXIT_FAILURE) {
        throw("thread_pool init failed");
    } else {
        return pool;
    }

error:
    if(pool) { pool->destroy(pool); };
    return NULL;
}

int Thread_Pool_init(Thread_Pool *this, const int max_threads)
{
	this->current_job = 0;
	this->max_threads = max_threads;

	this->threads = NEW(Linkedlist);
    check_mem(this->threads);


    // pthread_mutexattr_t pthread_attr;
    // pthread_mutexattr_init(&pthread_attr);
    // pthread_mutexattr_settype(&pthread_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&(this->jobs_mutex), NULL);

    this->jobs = NEW(Linkedlist);
    check_mem(this->jobs);

    this->thread_count = 0;
    this->working_threads = 0;

    for(int i = 0; i < max_threads; i++)
    {
    	/**
		* add a new thread for use
		*/
		Thread * current_thread = NEW(Thread, this->thread_count, this->jobs, &this->jobs_mutex);
		check_mem(current_thread);

		this->threads->append(this->threads, (void *)current_thread, sizeof(Thread));
		this->thread_count++;

		Thread * last_thread = (Thread *) this->threads->get(this->threads, i);
		last_thread->start(last_thread);
		
		current_thread->destroy(current_thread);
    }

	return EXIT_SUCCESS;

error:
	return EXIT_FAILURE;
}

void Thread_Pool_destroy(Thread_Pool *this)
{
	if(this){
        Linkednode * curr = this->threads->head;    
        while(curr){
            Thread * thread = (Thread *)curr->get(curr);
            thread->destroy(thread);
            curr->value = NULL;
            curr = curr->next;
        }
        if(this->threads) { this->threads->destroy(this->threads); };

        pthread_mutex_lock(&this->jobs_mutex);
		curr = this->jobs->head;        
        while(curr){
            Job * job = (Job *)curr->get(curr);
            if(job){
                job->destroy(job);
                curr->value = NULL;
            }
            curr = curr->next;
        }
		if(this->jobs) { this->jobs->destroy(this->jobs); };
        pthread_mutex_unlock(&this->jobs_mutex);
		
        pthread_mutex_destroy(&this->jobs_mutex);
        
        free(this);
	}
}

void Thread_Pool_add_job(Thread_Pool *this, Job *job)
{
	/**
	* determine whether need a new thread
	* or use an old thread
	*/

    pthread_mutex_lock(&this->jobs_mutex);
	this->jobs->append(this->jobs, (void *)job, sizeof(Job));
    pthread_mutex_unlock(&this->jobs_mutex);
}

void Thread_Pool_print(Thread_Pool *this)
{

}