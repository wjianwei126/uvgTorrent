#ifndef _tracker_h
#define _tracker_h

#include "macro/macro.h"
#include "data_structures/linkedlist/linkedlist.h"

typedef struct Tracker Tracker;
struct Tracker {
    int (*init)(Tracker *this, char *address);
    void (*print)(Tracker *this);
    void (*destroy)(Tracker *this);
    
    int (*connect)(Tracker *this);

    char * url;
    char * ip;
    char * port;
    int connected;
};

/* basic functions */
Tracker *Tracker_new(size_t size, char *address);
int Tracker_init(Tracker *this, char *address);
void Tracker_destroy(Tracker *this);
void Tracker_print(Tracker *this);

int Tracker_connect(Tracker *this);
#endif