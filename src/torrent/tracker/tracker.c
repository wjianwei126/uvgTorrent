#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils/string_utils.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "torrent/tracker/tracker.h"

Tracker *Tracker_new(size_t size, char *address)
{
	Tracker *tracker = malloc(size);
    check_mem(tracker);

    tracker->init = Tracker_init;
    tracker->destroy = Tracker_destroy;
    tracker->print= Tracker_print;
    tracker->announce = Tracker_announce;

    if(tracker->init(tracker, address) == EXIT_FAILURE) {
        throw("tracker init failed");
    } else {
        // all done, we made an object of any type
        return tracker;
    }

error:
    if(tracker) { tracker->destroy(tracker); };
    return NULL;
}

int Tracker_init(Tracker *this, char *address)
{
	this->port = NULL;

	char * addr = string_utils.urldecode(address);
	Linkedlist * url_port = string_utils.split(addr, ':');

    const char * protocol = (char *) url_port->get(url_port, 0);
    const char * url = (char *) url_port->get(url_port, 1);
    const char * port = (char *) url_port->get(url_port, 2);

    this->port = malloc(strlen(port) + 1);
    check_mem(this->port);
    strcpy(this->port, port);

    this->url = malloc(strlen(protocol) + strlen(":") + strlen(url) + 1);
    check_mem(this->url);
    strcpy(this->url, protocol);
    strcat(this->url, ":");
    strcat(this->url, url);

    url_port->destroy(url_port);
    free(addr);
    
	return EXIT_SUCCESS;
error:
	return EXIT_FAILURE;
}

void Tracker_destroy(Tracker *this)
{
	if(this){
		if(this->port) { free(this->port); }
		if(this->url) { free(this->url); }
		free(this);
	}
}

void Tracker_print(Tracker *this)
{
	debug("Torrent Tracker :: %s:%s", this->url, this->port);
}

void Tracker_announce(Tracker *this)
{
	log_info("TRACKER ANNOUNCE");
}