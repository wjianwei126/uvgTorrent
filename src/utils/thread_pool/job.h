#ifndef _job_h
#define _job_h

#include "macro/macro.h"

typedef struct Job Job;

struct Job {
    int (*init)(Job *this, void * (*function) (void* arg), void * arg);
    void (*print)(Job *this);
    void (*destroy)(Job *this);
   	
   	void* (*function)(void* arg);
	void* arg;
};

Job *Job_new(size_t size, void * (*function) (void* arg), void * arg);
int Job_init(Job *this, void * (*function) (void* arg), void * arg);
void Job_destroy(Job *this);
void Job_print(Job *this);

#endif