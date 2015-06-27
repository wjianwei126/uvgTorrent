#include "macro/macro.h"

#ifndef _torrent_h
#define _torrent_h

typedef struct {
    int (*init)(void *self, char *path);
    void (*print)(void *self);
    void (*destroy)(void *self);
    
    char * path;
    int (*parse)(void *self);
} Torrent;

/* constructor / destructor functions */
void *Torrent_new(size_t size, char *path);
int Torrent_init(void *self, char *path);
void Torrent_print(void *self);
void Torrent_destroy(void *self);

/* */
int Torrent_parse(void *self);

#endif