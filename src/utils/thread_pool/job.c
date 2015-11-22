#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils/thread_pool/job.h"

Job *Job_new(size_t size, void * (*function) (void* arg), void * arg)
{
	Job *job = malloc(size);
    check_mem(job);

    job->init = Job_init;
    job->destroy = Job_destroy;
    job->print= Job_print;

    if(job->init(job, function, arg) == EXIT_FAILURE) {
        throw("job init failed");
    } else {
        return job;
    }

error:
    if(job) { job->destroy(job); };
    return NULL;
}

int Job_init(Job *this, void * (*function) (void* arg), void * arg)
{
	this->function = function;
	this->arg = arg;
	
	return EXIT_SUCCESS;
}

void Job_destroy(Job *this)
{
	if(this){
        free(this);
	}
}

void Job_print(Job *this)
{
    debug("JOB");
}