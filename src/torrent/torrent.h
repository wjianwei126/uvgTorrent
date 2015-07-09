#ifndef _torrent_h
#define _torrent_h

#include "macro/macro.h"
#include "data_structures/linkedlist/linkedlist.h"

typedef struct Torrent Torrent;
struct Torrent {
    int (*init)(Torrent *this, char *path);
    void (*print)(Torrent *this);
    void (*destroy)(Torrent *this);
    
    int (*parse)(Torrent *this);
    void (*announce)(Torrent *this);

    char * path;
    char * name;
    char * hash;
    Linkedlist * trackers;
};

/* basic functions */
Torrent *Torrent_new(size_t size, char *path);
int Torrent_init(Torrent *this, char *path);
void Torrent_destroy(Torrent *this);
void Torrent_print(Torrent *this);

int Torrent_parse(Torrent *this);
void Torrent_announce(Torrent *this);

#endif