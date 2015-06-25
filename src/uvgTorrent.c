/*
* uvgTorrent main file
* Author: Simon Bursten (unovongalixor :: github)
*/

#include "debug.h"
#include <stdlib.h>
#include <stdio.h>

#include "torrent.h"

int main(int argc, char *argv[])
{
    /* initialize torrent_path as null
       will help determine if pointer needs to be freed on error */
    char * torrent_path = NULL;

    /* verify user has provided a torrent to parse */
    assert(argc == 2, "provide a torrent to download");
    
    /* copy torrent path */
    size_t torrent_path_len = strlen(argv[1]);
    torrent_path = malloc(torrent_path_len+1);
    /* verify memory was allocated */
    check_mem(test); 
    strcpy(torrent_path, argv[1]);

    /* alert user parsing is underway */
    log_info("attempting to parse torrent file :: %s", torrent_path);

    /* cleanup */
    free(torrent_path);
    return 0;

error:
    /* cleanup */
    if(torrent_path) { free(torrent_path); };
    return 1;
}