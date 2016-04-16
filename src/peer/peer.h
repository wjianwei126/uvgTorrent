#ifndef _peer_h
#define _peer_h

#include "macro/macro.h"
#include <inttypes.h>

typedef struct Peer Peer;
struct Peer {
    int (*init)(Peer *this, char *ip, uint16_t port);
    void (*print)(Peer *this);
    void (*destroy)(Peer *this);
    
    void (*handshake)(Peer *this);

    char * ip;
    uint16_t port;
    int am_choking; // this client is choking the peer
    int am_interested; // this client is interested in the peer
    int peer_choking; // peer is choking this client
    int peer_interested; // peer is interested in this client
};

/* basic functions */
Peer *Peer_new(size_t size, char *ip, uint16_t port);
int Peer_init(Peer *this, char *ip, uint16_t port);
void Peer_destroy(Peer *this);
void Peer_print(Peer *this);

void Peer_handshake(Peer *this);

#endif