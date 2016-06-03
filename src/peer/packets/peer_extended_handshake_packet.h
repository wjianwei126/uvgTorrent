#ifndef _peer_extended_handshake_packet_h
#define _peer_extended_handshake_packet_h

#include "macro/macro.h"
#include "utils/sock/socket.h"

typedef struct Peer_Extended_Handshake_Request Peer_Extended_Handshake_Request;
struct Peer_Extended_Handshake_Request { 
	int (*init) (Peer_Extended_Handshake_Request *this);
    void (*print) (Peer_Extended_Handshake_Request *this);
    void (*destroy) (Peer_Extended_Handshake_Request *this);

	char bytes[30];
};

Peer_Extended_Handshake_Request *Peer_Extended_Handshake_Request_new (size_t size);
int Peer_Extended_Handshake_Request_init (Peer_Extended_Handshake_Request *this);
void Peer_Extended_Handshake_Request_destroy (Peer_Extended_Handshake_Request *this);
void Peer_Extended_Handshake_Request_print (Peer_Extended_Handshake_Request *this);

/* PEER HANDSHAKE RESPONSE */
typedef struct Peer_Extended_Handshake_Response Peer_Extended_Handshake_Response;
struct Peer_Extended_Handshake_Response { 
	int (*init) (Peer_Extended_Handshake_Response *this, char raw_response[2048], ssize_t res_size);
    void (*print) (Peer_Extended_Handshake_Response *this);
    void (*destroy) (Peer_Extended_Handshake_Response *this);

    int ut_metadata;
    int metadata_size;
    int num_pieces;
    int piece_size;
};

Peer_Extended_Handshake_Response *Peer_Extended_Handshake_Response_new (size_t size, char raw_response[2048], ssize_t res_size);
int Peer_Extended_Handshake_Response_init (Peer_Extended_Handshake_Response *this, char raw_response[2048], ssize_t res_size);
void Peer_Extended_Handshake_Response_destroy (Peer_Extended_Handshake_Response *this);
void Peer_Extended_Handshake_Response_print (Peer_Extended_Handshake_Response *this);

/* PEER HANDSHAKE WRAPPER */
typedef struct Peer_Extended_Handshake_Packet Peer_Extended_Handshake_Packet;
struct Peer_Extended_Handshake_Packet {
	int (*init) (Peer_Extended_Handshake_Packet *this);
    void (*print) (Peer_Extended_Handshake_Packet *this);
    void (*destroy) (Peer_Extended_Handshake_Packet *this);

	int (*send) (Peer_Extended_Handshake_Packet *this, Socket * socket);
	int (*receive) (Peer_Extended_Handshake_Packet *this, Socket * socket);

	Peer_Extended_Handshake_Request * request;
	Peer_Extended_Handshake_Response * response;
};

Peer_Extended_Handshake_Packet * Peer_Extended_Handshake_Packet_new (size_t size);
int Peer_Extended_Handshake_Packet_init (Peer_Extended_Handshake_Packet *this);
void Peer_Extended_Handshake_Packet_destroy (Peer_Extended_Handshake_Packet *this);
void Peer_Extended_Handshake_Packet_print (Peer_Extended_Handshake_Packet *this);

int Peer_Extended_Handshake_Packet_send (Peer_Extended_Handshake_Packet *this, Socket * socket);
int Peer_Extended_Handshake_Packet_receive (Peer_Extended_Handshake_Packet *this, Socket * socket);

#endif