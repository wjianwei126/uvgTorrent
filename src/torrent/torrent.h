#include "macro/macro.h"
#include "hashmap/hashmap.h"

#ifndef _torrent_h
#define _torrent_h

// type
typedef struct Torrent Torrent;
struct Torrent {
    int (*init)(Torrent *this, char *path);
    void (*print)(Torrent *this);
    void (*destroy)(Torrent *this);
    
    int (*parse)(Torrent *this);

    char * path;
    Hashmap * magnet_data;
};

/* constructor / destructor functions */
Torrent *Torrent_new(size_t size, char *path);
int Torrent_init(Torrent *this, char *path);
void Torrent_print(Torrent *this);
void Torrent_destroy(Torrent *this);

int Torrent_parse(Torrent *this);

#endif