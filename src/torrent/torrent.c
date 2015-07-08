#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "hashmap/hashmap.h"
#include "torrent/torrent.h"

Torrent *Torrent_new(size_t size, char *path)
{
    Torrent *torrent = malloc(size);
    check_mem(torrent);

    torrent->init = Torrent_init;
    torrent->destroy = Torrent_destroy;
    torrent->print= Torrent_print;

    torrent->parse = Torrent_parse;

    if(torrent->init(torrent, path) == EXIT_FAILURE) {
        throw("torrent init failed");
    } else {
        // all done, we made an object of any type
        return torrent;
    }

error:
    if(torrent) { torrent->destroy(torrent); };
    return NULL;
}

/**
* int Torrent_init(Torrent *this, char *path)
*
* Torrent    *this; instance to initialize
* char       path; path to file containing magnet link 
* 
* PURPOSE : copy torrent path into Torrent struct
* RETURN  : success bool
* NOTES   : magnet link will be taken from stdin
*           once app is working
*/
int Torrent_init(Torrent *this, char *path)
{   
	this->path = NULL;

	this->path = malloc(strlen(path) + 1);
    check_mem(this->path);
    strcpy(this->path, path);

    return EXIT_SUCCESS;
error:
	return EXIT_FAILURE;
}

void Torrent_destroy(Torrent * this)
{   
    if(this) {
        if(this->path) { free(this->path);  };
        if(this->magnet_data) { this->magnet_data->destroy(this->magnet_data); };
        free(this);
    }
}

/**
* void Torrent_print(Torrent *this){
*
* PURPOSE : print a torrent struct
*/
void Torrent_print(Torrent *this){
    if(this){

    }
}

/*
* int Torrent_parse(Torrent *this)
* 
* PURPOSE : parse a torrent objects magnet url from a file
* RETURN  : success bool
* NOTES   : if file was successfully parsed the torrents 
*           magnet_data hashmap will be ready for use
*
*/
int Torrent_parse(Torrent *this){
    FILE *torrent_file = NULL;
	char *torrent_content = NULL;

    log_info("attempting to parse torrent file :: %s", this->path);

    torrent_file = fopen(this->path, "rb");
    if (!torrent_file){
    	/* No such file or directory (POSIX.1) */
    	errno = ENOENT;
    	throw("%s", this->path);
    }

    /* get file size */
    fseek(torrent_file, 0, SEEK_END);
    long torrent_size = ftell(torrent_file);
    rewind(torrent_file);

    torrent_content = malloc(torrent_size+1);
    check_mem(torrent_content);

    if (!fread(torrent_content, sizeof(char), torrent_size, torrent_file)) 
    { 
        throw("read torrent failed");
    }
    torrent_content[torrent_size] = '\0';

    log_info("%s",torrent_content);
    
    this->magnet_data = NEW(Hashmap);
    check_mem(this->magnet_data);
    
    /*
    char *test_val = "1234";
    char *test_val2 = "1234567";
    if(this->magnet_data->set(this->magnet_data, "test", test_val, sizeof(char) * strlen(test_val)) == EXIT_FAILURE){
        throw("magnet_data set failed");
    }
    if(this->magnet_data->set(this->magnet_data, "test2", test_val2, sizeof(char) * strlen(test_val2)) == EXIT_FAILURE){
        throw("magnet_data set failed");
    }
    const char * val = this->magnet_data->get(this->magnet_data, "test");
    log_info("%s", val);
    const char * val2 = this->magnet_data->get(this->magnet_data, "test2");
    log_info("%s", val2);
    */

	/* cleanup */
    fclose(torrent_file);
	free(torrent_content);
    
	return EXIT_SUCCESS;

error:
	/* cleanup */
	if(torrent_content) { free(torrent_content); };
    if(torrent_file) { fclose(torrent_file); };
    
    log_err("failed to parse torrent file :: %s", this->path);

	return EXIT_FAILURE;
}