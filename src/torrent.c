#include "debug.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "torrent.h"

void Torrent_destroy(void *self)
{
    Torrent *torrent = self;

    if(torrent) {
    	if(torrent->path) { free(torrent->path); };
       	free(torrent);
    }
}

int Torrent_init(void *self, char *path)
{
	Torrent * torrent = self;
	torrent->path = NULL;

	torrent->path = malloc(strlen(path) + 1);
    check_mem(torrent->path);
    strcpy(torrent->path, path);

    return 1;

error:
	return 0;
}

void *Torrent_new(size_t size, char *path)
{
    Torrent *torrent = malloc(size);
    check_mem(torrent);

    torrent->init = Torrent_init;
    torrent->destroy = Torrent_destroy;
    torrent->parse = Torrent_parse;

    if(!torrent->init(torrent, path)) {
        // looks like it didn't initialize properly
        torrent->destroy(torrent);
        return NULL;
    } else {
        // all done, we made an object of any type
        return torrent;
    }

error:
	return NULL;
}


int Torrent_parse(void *self){
	Torrent * torrent = self;

    /* alert user parsing is underway */
    log_info("attempting to parse torrent file :: %s", torrent->path);

    goto error;
    
	return 1;

error:
    /* alert user parsing is underway */
    log_err("failed to parse torrent file :: %s", torrent->path);

	return 0;
}