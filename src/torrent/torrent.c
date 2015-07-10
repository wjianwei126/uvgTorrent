#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "utils/str/string_utils.h"
#include "data_structures/hashmap/hashmap.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "torrent/torrent.h"
#include "torrent/tracker/tracker.h"

Torrent *Torrent_new(size_t size, char *path)
{
    Torrent *torrent = malloc(size);
    check_mem(torrent);

    torrent->init = Torrent_init;
    torrent->destroy = Torrent_destroy;
    torrent->print= Torrent_print;

    torrent->parse = Torrent_parse;
    torrent->announce = Torrent_announce;

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
    this->name = NULL;
    this->hash = NULL;
    this->trackers = NULL;

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
        if(this->name) { free(this->name);  };
        if(this->hash) { free(this->hash);  };

        /* 
            linked node stores structs as void *
            therefore they must be casted and destroyed
        */
        Linkednode * curr = this->trackers->head;        
        while(curr){
            Tracker * tracker = (Tracker *)curr->get(curr);
            tracker->destroy(tracker);
            curr->value = NULL;
            curr = curr->next;
        }

        if(this->trackers) { this->trackers->destroy(this->trackers); };
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
        debug("Torrent Name :: %s", this->name);
        debug("Torrent Hash :: %s", this->hash);

        Linkednode * curr = this->trackers->head;        
        while(curr){
            Tracker * tracker = (Tracker *)curr->value;
            tracker->print(tracker);
            curr = curr->next;
        }
        
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

    log_confirm("attempting to parse torrent file :: %s", this->path);

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

    char * sub_string = torrent_content + 8;

    Hashmap *hashmap = NULL;
    hashmap = NEW(Hashmap);
    check_mem(hashmap);
    this->trackers = NEW(Linkedlist);
    check_mem(this->trackers);

    /* temporary buffers */
    char key_buffer[256];
    char value_buffer[256];
    memset(key_buffer, 0, 256);
    memset(value_buffer, 0, 256);

    int str_pos = 0; /* pos in magnet uri */
    int key = 1; /* are we extracting a key or a value? */
    int input_len = strlen(sub_string) - 1;
    /* remove any solo key value pairs */
    /* this code skips over trackers as there may be many sharing the same key */
    for (int i = 0; sub_string[i] != '\0'; i++){
        switch(key){
            case 1:
                if(sub_string[i] != '=' && str_pos < 256 && i < input_len){
                    key_buffer[str_pos] = sub_string[i];
                    str_pos++;
                } else {
                    key = 0;
                    str_pos = 0;
                }
                break;
            case 0:
                if(sub_string[i] != '&' && str_pos < 256 && i < input_len){
                    value_buffer[str_pos] = sub_string[i];
                    str_pos++;
                } else {
                    key = 1;
                    str_pos = 0;
                    if(strcmp(key_buffer, "tr") != 0){
                        if(hashmap->set(hashmap, key_buffer, value_buffer, strlen(value_buffer)) == EXIT_FAILURE){
                            throw("parsing failed");
                        }
                    } else {
                        Tracker *tracker = NEW(Tracker, value_buffer);
                        if(this->trackers->append(this->trackers, tracker, sizeof(Tracker)) == EXIT_FAILURE){
                            throw("parsing failed");
                        }
                        free(tracker);
                    }
                    memset(key_buffer, 0, 256);
                    memset(value_buffer, 0, 256);
                }
                break;
        }
    }

    /* name copy */
    this->name = NULL;
    const char * dn = hashmap->get(hashmap, "dn");
    this->name = string_utils.urldecode((char *) dn);
    check_mem(this->name);

    /* hash copy */
    this->hash = NULL;
    const char * xt = hashmap->get(hashmap, "xt");
    this->hash = malloc(strlen(xt) + 1);
    check_mem(this->hash);
    strcpy(this->hash, xt);

	/* cleanup */
    hashmap->destroy(hashmap);
    fclose(torrent_file);
	free(torrent_content);
    fprintf(stderr, " %s✔%s\n", KGRN, KNRM);

	return EXIT_SUCCESS;

error:
	/* cleanup */
    if(hashmap) { hashmap->destroy(hashmap); };
	if(torrent_content) { free(torrent_content); };
    if(torrent_file) { fclose(torrent_file); };
    if(this->name) { free(this->name); };
    if(this->hash) { free(this->hash); };
    
    log_err("failed to parse torrent file :: %s", this->path);

	return EXIT_FAILURE;
}

void Torrent_announce(Torrent *this)
{
    Linkednode * curr = this->trackers->head;  
    while(curr){
        Tracker * tracker = (Tracker *)curr->get(curr);
        tracker->announce(tracker);
        curr = curr->next;
    }
}