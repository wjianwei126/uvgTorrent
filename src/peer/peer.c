#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "utils/sock/socket.h"
#include "peer/peer.h"
#include "peer/packets/peer_handshake_packet.h"

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
int Peer_handshake(Peer *this, char * info_hash){
    log_confirm("Attempting Handshake :: %s:%u", this->ip, this->port);
    Socket * socket = NEW(Socket, this->ip, this->port, SOCKET_TYPE_TCP);
    check_mem(socket);

    Peer_Handshake_Packet * handshake = NULL;

    int result = socket->connect(socket);
    if(result == EXIT_SUCCESS){
    	handshake = NEW(Peer_Handshake_Packet, info_hash, "-UVG012345678912345-");
        int attempts = 0;
        int success = 0;
        while(attempts < 5){
            attempts++;
        	if(handshake->send(handshake, socket) == EXIT_SUCCESS){
                int result = EXIT_FAILURE;
                result = handshake->receive(handshake, socket);

                if(result == EXIT_SUCCESS){
                    success = 1;
                    fprintf(stderr, " %s✔%s\n", KGRN, KNRM);
                    break;
                }
            } else {
                socket->destroy(socket);
                goto error;
            }
        }
        handshake->destroy(handshake);
        if (success == 0) {
            fprintf(stderr, " %s✘%s\n", KRED, KNRM);
        }
    } else {
        socket->destroy(socket);
    	goto error;
    }
    socket->destroy(socket);
	
	return EXIT_SUCCESS;

error:
	if(handshake) { handshake->destroy(handshake); };
    fprintf(stderr, " %s✘%s\n", KRED, KNRM);
	return EXIT_FAILURE;
}