/*
* uvgTorrent main file
* Author: Simon Bursten (unovongalixor :: github)
*/

/* GLOBAL NEW MACRO 
*  initializes a struct based on convention: 
*  NEW(TYPE, ARGS) -> TYPE_new(sizeof(TYPE), ARGS);
*/

#define NEW(T, ...) T ##_new(sizeof(T), ##__VA_ARGS__)

#include "debug.h"
#include <stdlib.h>
#include <stdio.h>

#include "torrent.h"

int main(int argc, char *argv[])
{
    Torrent *torrent = NULL;
    /* verify user has provided a torrent to parse */
    assert(argc == 2, "provide a torrent to download");
    
    torrent = NEW(Torrent, argv[1]);
    check_mem(torrent);

    if(torrent->parse(torrent)){

    }

    /* cleanup */
    torrent->destroy(torrent);
    return 0;

error:
    /* cleanup */
    if(torrent) { torrent->destroy(torrent); };
    return 1;
}