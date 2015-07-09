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

	string_utils.urldecode(address);
    log_info("%s", address);

	Tracker *tracker = malloc(size);
    check_mem(tracker);

    tracker->init = Tracker_init;
    tracker->destroy = Tracker_destroy;
    tracker->print= Tracker_print;

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
	return EXIT_SUCCESS;
}

void Tracker_destroy(Tracker *this)
{

}

void Tracker_print(Tracker *this)
{

}