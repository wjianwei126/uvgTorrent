#include "debug/debug.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures/hashmap/hashmap.h"

/*
* extern Hashmap * magnet_parse(const char * input)
* 
* PURPOSE : parse a magnet link into an easy to use Hashmap object
* RETURN  : parsed key value pairs
* NOTES   : function is only appropriate for magnet links
*
*/
extern Hashmap * magnet_parse(const char * input){
	Hashmap *hashmap = NULL;
	hashmap = NEW(Hashmap);
    check_mem(hashmap);

    char key_buffer[256];
    char value_buffer[256];
    memset(key_buffer, 0, 256);
    memset(value_buffer, 0, 256);

    int str_pos = 0;
    int key = 1;

    char trackers[10][256];
    int tracker = 0;
    memset(trackers, 0, 10*256);

    /* remove any solo key value pairs */
    /* this code skips over trackers as there may be many sharing the same key */
    for (int i = 0; input[i] != '\0'; i++){
        switch(key){
        	case 1:
	            if(input[i] != '='){
	                key_buffer[str_pos] = input[i];
	                str_pos++;
	            } else {
	                key = 0;
	                str_pos = 0;
	            }
	            break;
        	case 0:
	            if(input[i] != '&'){
	                value_buffer[str_pos] = input[i];
	                str_pos++;
	            } else {
	                key = 1;
	                str_pos = 0;
	                if(strcmp(key_buffer, "tr") != 0){
	                	if(hashmap->set(hashmap, key_buffer, value_buffer, strlen(value_buffer)) == EXIT_FAILURE){
	                		throw("parsing failed");
	                	}
	                } else {
	                	if(tracker < 10){
	                		if(strlen(value_buffer) < 256) { memcpy(trackers[tracker], value_buffer, strlen(value_buffer)); }
	                		tracker++;
	                	}
	                }
	                memset(key_buffer, 0, 256);
	                memset(value_buffer, 0, 256);
	            }
	            break;
	    }
    }

    hashmap->set(hashmap, "tr", trackers, 256*10);
    
    return hashmap;
error:
	if(hashmap != NULL) { hashmap->destroy(hashmap); }
	return hashmap;
}