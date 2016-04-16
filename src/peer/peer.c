#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "peer/peer.h"

Peer *Peer_new(size_t size, char *ip, uint16_t port)
{
    Peer *Peer = malloc(size);
    check_mem(Peer);

    Peer->init = Peer_init;
    Peer->destroy = Peer_destroy;
    Peer->print= Peer_print;

    Peer->handshake = Peer_handshake;

    if(Peer->init(Peer, ip, port) == EXIT_FAILURE) {
        throw("Peer init failed");
    } else {
        // all done, we made an object of any type
        return Peer;
    }

error:
    if(Peer) { Peer->destroy(Peer); };
    return NULL;
}

/**
* int Peer_init(Peer *this, char *path)
*
* Peer    *this; instance to initialize
* char       path; path to file containing magnet link 
* 
* PURPOSE : copy Peer path into Peer struct
* RETURN  : success bool
* NOTES   : magnet link will be taken from stdin
*           once app is working
*/
int Peer_init(Peer *this, char *ip, uint16_t port)
{   
	this->ip = NULL;
    this->port = port;

	this->ip = malloc(strlen(ip) + 1);
    check_mem(this->ip);
    strcpy(this->ip, ip);

    return EXIT_SUCCESS;

error:
	return EXIT_FAILURE;
}

void Peer_destroy(Peer * this)
{   
    if(this) {
        if(this->ip) { free(this->ip);  };
        free(this);
    }
}

/**
* void Peer_print(Peer *this){
*
* PURPOSE : print a Peer struct
*/
void Peer_print(Peer *this){
    if(this){
        debug("peer :: %s:%u", this->ip, this->port);
    }
}

/**
* void Peer_print(Peer *this){
*
* PURPOSE : print a Peer struct
*/
void Peer_handshake(Peer *this){
    this->print(this);	
}