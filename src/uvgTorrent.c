/*
* uvgTorrent main file
* Author: Simon Bursten (unovongalixor :: github)
*/
#include "macro/macro.h"
#include "debug/debug.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

#include "utils/thread_pool/thread_pool.h"
#include "torrent/torrent.h"


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
    thread_pool = NEW(Thread_Pool, 100);
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

int main(int argc, char *argv[])
{
    return threadpool();
//     Torrent *torrent = NULL;
//     /* verify user has provided a torrent to parse */
//     assert(argc == 2, "provide a torrent to download");
//     
//     torrent = NEW(Torrent, argv[1]);
//     check_mem(torrent);
// 
//     if(torrent->parse(torrent) == EXIT_SUCCESS){
//         /* print parsed torrent info */
//         torrent->print(torrent);
// 
//         /* connect to trackers */
//         int connected = torrent->connect(torrent);
//         if(connected == EXIT_FAILURE){
//             throw("torrent failed to connect to trackers");
//         }
// 
//         /* announce to trackers */
//         torrent->announce(torrent);
//     }
// 
//     /* cleanup */
//     torrent->destroy(torrent);
//     return EXIT_SUCCESS;
// 
// error:
//     /* cleanup */
//     if(torrent) { torrent->destroy(torrent); };
//     return EXIT_FAILURE;
}