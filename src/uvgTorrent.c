/*
* uvgTorrent main file
* Author: Simon Bursten (unovongalixor :: github)
*/
#include "macro/macro.h"
#include "debug/debug.h"
#include <stdlib.h>
#include <stdio.h>

#include "torrent/torrent.h"
#include "data_structures/linkedlist/linkedlist.h"

int main(int argc, char *argv[])
{
    Torrent *torrent = NULL;
    /* verify user has provided a torrent to parse */
    assert(argc == 2, "provide a torrent to download");

    Linkedlist * list = NEW(Linkedlist);
    list->append(list, "test\0", strlen("test\0"));
    list->append(list, "test1\0", strlen("test1\0"));
    list->append(list, "test2\0", strlen("test2\0"));
    list->append(list, "test3\0", strlen("test3\0"));

    log_info("%s", (char *) list->get(list, 0));
    log_info("%s", (char *) list->get(list, 1));
    log_info("%s", (char *) list->get(list, 2));
    log_info("%s", (char *) list->get(list, 3));
    
    list->destroy(list);

    torrent = NEW(Torrent, argv[1]);
    check_mem(torrent);

    if(torrent->parse(torrent)){
        /* print parsed torrent info */
        torrent->print(torrent);
    }

    /* cleanup */
    torrent->destroy(torrent);
    return EXIT_SUCCESS;

error:
    /* cleanup */
    if(torrent) { torrent->destroy(torrent); };
    return EXIT_FAILURE;
}