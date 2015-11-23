/**
*   ThreadPool example - Simon Bursten
*   
*   # Notes #
*   I'm leaving this here until I get around to
*   writing proper documentation for the thread pool
*
*   # Overview #
*   The threadpool manages a pool of threads, and a queue
*   of available jobs to be run. 
*   
*   the thread pool uses a mutex to syncronize access to the 
*   job queue, and handles creating and destroying threads as 
*   needed
*/

/*
void task1(void * arg){
    int argument = (int *)arg;
    printf("#%d working on task1\n", argument);
}

void task2(void * arg){
    int argument = (int *)arg;
    printf("#%d working on task2\n", argument);
}

Thread_Pool *thread_pool = NULL;
int running = 1;

// catch ctrl-c
void cleanup() {
    running = 0;
}

int threadpool() {
    signal(SIGINT, cleanup);
    
    debug("Making threadpool with 4 threads");
    thread_pool = NEW(Thread_Pool, 4);
    check_mem(thread_pool);
    
    debug("Adding 50 tasks to threadpool");
    int i;
    for (i=0; i<25; i++){
        Job * job1 = NEW(Job, (void*)task1, (void *)2);
        Job * job2 = NEW(Job, (void*)task2, (void *)10);

        thread_pool->add_job(thread_pool, job1);
        thread_pool->add_job(thread_pool, job2);

        job1->destroy(job1);
        job2->destroy(job2);
    };
    
    while(running){
        
    }

    if(thread_pool) { thread_pool->destroy(thread_pool); };
error:
     return EXIT_FAILURE;
}
*/