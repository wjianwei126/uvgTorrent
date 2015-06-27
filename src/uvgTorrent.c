/*
* uvgTorrent main file
* Author: Simon Bursten (unovongalixor :: github)
*/
#include "macro/macro.h"
#include "debug/debug.h"
#include <stdlib.h>
#include <stdio.h>

#include "torrent/torrent.h"

int main(int argc, char *argv[])
{
    Torrent *torrent = NULL;
    /* verify user has provided a torrent to parse */
    assert(argc == 2, "provide a torrent to download");
    
    torrent = NEW(Torrent, argv[1]);
    check_mem(torrent);

    if(torrent->parse(torrent)){
        /* print parsed torrent info */
        torrent->print(torrent);
    }

    /* cleanup */
    torrent->destroy(torrent);
    return 0;

error:
    /* cleanup */
    if(torrent) { torrent->destroy(torrent); };
    return 1;
}