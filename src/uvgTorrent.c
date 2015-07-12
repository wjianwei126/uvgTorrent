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

    if(torrent->parse(torrent) == EXIT_SUCCESS){
        /* print parsed torrent info */
        torrent->print(torrent);

        /* connect to trackers */
        int connected = torrent->connect(torrent);
        if(connected == EXIT_FAILURE){
            throw("torrent failed to connect to trackers");
        }

        /* announce to trackers */
        torrent->announce(torrent);
    }

    /* cleanup */
    torrent->destroy(torrent);
    return EXIT_SUCCESS;

error:
    /* cleanup */
    if(torrent) { torrent->destroy(torrent); };
    return EXIT_FAILURE;
}