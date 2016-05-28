#ifndef _peer_handshake_packet_h
#define _peer_handshake_packet_h

#include "macro/macro.h"
#include "utils/sock/socket.h"

/* PEER HANDSHAKE REQUEST */
typedef struct Peer_Handshake_Request Peer_Handshake_Request;
struct Peer_Handshake_Request { 
	int (*init) (Peer_Handshake_Request *this, char * info_hash, char * peer_id);
    void (*print) (Peer_Handshake_Request *this);
    void (*destroy) (Peer_Handshake_Request *this);

	int8_t pstrlen;
	char * pstr;
	char * info_hash;
	char * peer_id;
	char bytes[68];
};

Peer_Handshake_Request *Peer_Handshake_Request_new (size_t size, char * info_hash, char * peer_id);
int Peer_Handshake_Request_init (Peer_Handshake_Request *this, char * info_hash, char * peer_id);
void Peer_Handshake_Request_destroy (Peer_Handshake_Request *this);
void Peer_Handshake_Request_print (Peer_Handshake_Request *this);

/* PEER HANDSHAKE RESPONSE */
typedef struct Peer_Handshake_Response Peer_Handshake_Response;
struct Peer_Handshake_Response { 
	int (*init) (Peer_Handshake_Response *this, char raw_response[2048], ssize_t res_size);
    void (*print) (Peer_Handshake_Response *this);
    void (*destroy) (Peer_Handshake_Response *this);

	int8_t pstrlen;
	char * pstr;
	char * peer_id;
};

Peer_Handshake_Response *Peer_Handshake_Response_new (size_t size, char raw_response[2048], ssize_t res_size);
int Peer_Handshake_Response_init (Peer_Handshake_Response *this, char raw_response[2048], ssize_t res_size);
void Peer_Handshake_Response_destroy (Peer_Handshake_Response *this);
void Peer_Handshake_Response_print (Peer_Handshake_Response *this);

/* PEER HANDSHAKE WRAPPER */
typedef struct Peer_Handshake_Packet Peer_Handshake_Packet;
struct Peer_Handshake_Packet {
	int (*init) (Peer_Handshake_Packet *this, char * info_hash, char * peer_id);
    void (*print) (Peer_Handshake_Packet *this);
    void (*destroy) (Peer_Handshake_Packet *this);

	int (*send) (Peer_Handshake_Packet *this, Socket * socket);
	int (*receive) (Peer_Handshake_Packet *this, Socket * socket);

	Peer_Handshake_Request * request;
	Peer_Handshake_Response * response;
};

Peer_Handshake_Packet * Peer_Handshake_Packet_new (size_t size, char * info_hash, char * peer_id);
int Peer_Handshake_Packet_init (Peer_Handshake_Packet *this, char * info_hash, char * peer_id);
void Peer_Handshake_Packet_destroy (Peer_Handshake_Packet *this);
void Peer_Handshake_Packet_print (Peer_Handshake_Packet *this);

int Peer_Handshake_Packet_send (Peer_Handshake_Packet *this, Socket * socket);
int Peer_Handshake_Packet_receive (Peer_Handshake_Packet *this, Socket * socket);
#endif