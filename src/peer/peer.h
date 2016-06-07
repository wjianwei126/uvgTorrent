#ifndef _peer_h
#define _peer_h

#include <inttypes.h>

#include "macro/macro.h"
#include "utils/sock/socket.h"

typedef struct Peer Peer;
struct Peer {
    int (*init)(Peer *this, char *ip, uint16_t port);
    void (*print)(Peer *this);
    void (*destroy)(Peer *this);
    
    int (*handshake)(Peer *this, char * info_hash);
    int (*extended_handshake)(Peer *this);
    int (*get_metadata)(Peer *this, char * out, int metadata_size);

    char * ip;
    uint16_t port;
    int am_choking; // this client is choking the peer
    int am_interested; // this client is interested in the peer
    int peer_choking; // peer is choking this client
    int peer_interested; // peer is interested in this client
    Socket * socket;

    // ut_metadata related stuff
    int ut_metadata;
    int metadata_size;
    int num_pieces;
    int piece_size;
};

/* basic functions */
Peer *Peer_new(size_t size, char *ip, uint16_t port);
int Peer_init(Peer *this, char *ip, uint16_t port);
void Peer_destroy(Peer *this);
void Peer_print(Peer *this);

int Peer_handshake(Peer *this, char * info_hash);
int Peer_extended_handshake(Peer *this);
int Peer_get_metadata(Peer *this, char * out, int metadata_size);

#endif