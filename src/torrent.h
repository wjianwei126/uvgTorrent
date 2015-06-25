#ifndef _torrent_h
#define _torrent_h

typedef struct {
    char *description;
    int (*init)(void *self);
    void (*describe)(void *self);
    void (*destroy)(void *self);
    void *(*move)(void *self);
    int (*attack)(void *self);
} Torrent;

int Torrent_init(void *self);
void Torrent_destroy(void *self);
void *Torrent_new(size_t size, Torrent proto, char *description);

#define NEW(T, N) Torrent(sizeof(T), T##Proto, N)
#define _(N) proto.N

#endif