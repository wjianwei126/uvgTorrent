/*
* uvgTorrent main file
* Author: Simon Bursten (unovongalixor :: github)
*/
#include "macro/macro.h"
#include "debug/debug.h"
#include <stdlib.h>
#include <stdio.h>

#include "utils/thread_pool/thread_pool.h"
#include "torrent/torrent.h"


void task1(){
    printf("Thread #%u working on task1\n", (int)pthread_self());
}

void task2(){
    printf("Thread #%u working on task2\n", (int)pthread_self());
}

int main(int argc, char *argv[])
{
    puts("Making threadpool with 4 threads");
    Thread_Pool *thread_pool = NULL;
    thread_pool = NEW(Thread_Pool, 4);

    puts("Adding 40 tasks to threadpool");
    int i;
    for (i=0; i<20; i++){
        Job * job1 = NEW(Job, (void*)task1, NULL);
        Job * job2 = NEW(Job, (void*)task2, NULL);

        thread_pool->add_job(thread_pool, job1);
        thread_pool->add_job(thread_pool, job2);

        job1->destroy(job1);
        job2->destroy(job2);
    };
    
    

    puts("Killing threadpool");
    thread_pool->destroy(thread_pool);

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
