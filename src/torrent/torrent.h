#include "macro/macro.h"

#ifndef _torrent_h
#define _torrent_h

// type
typedef struct Torrent Torrent;
struct Torrent {
    int (*init)(Torrent *this, char *path);
    void (*print)(Torrent *this);
    void (*destroy)(Torrent *this);
    
    char * path;
    int (*parse)(Torrent *this);
};

/* constructor / destructor functions */
Torrent *Torrent_new(size_t size, char *path);
int Torrent_init(Torrent *this, char *path);
void Torrent_print(Torrent *this);
void Torrent_destroy(Torrent *this);

int Torrent_parse(Torrent *this);

#endif