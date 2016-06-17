#ifndef _peer_metadata_packet_h
#define _peer_metadata_packet_h

#include "macro/macro.h"
#include "utils/sock/socket.h"

/* PEER HANDSHAKE REQUEST */
typedef struct Peer_Metadata_Request Peer_Metadata_Request;
struct Peer_Metadata_Request { 
	int (*init) (Peer_Metadata_Request *this, int piece_num, int ut_metadata);
    void (*print) (Peer_Metadata_Request *this);
    void (*destroy) (Peer_Metadata_Request *this);

	int piece;
	char bytes[31];
};

Peer_Metadata_Request *Peer_Metadata_Request_new (size_t size, int piece_num, int ut_metadata);
int Peer_Metadata_Request_init (Peer_Metadata_Request *this, int piece_num, int ut_metadata);
void Peer_Metadata_Request_destroy (Peer_Metadata_Request *this);
void Peer_Metadata_Request_print (Peer_Metadata_Request *this);

/* PEER HANDSHAKE RESPONSE */
typedef struct Peer_Metadata_Response Peer_Metadata_Response;
struct Peer_Metadata_Response { 
	int (*init) (Peer_Metadata_Response *this, char raw_response[2048], ssize_t res_size);
    void (*print) (Peer_Metadata_Response *this);
    void (*destroy) (Peer_Metadata_Response *this);

	char * response;
	int response_len;
};

Peer_Metadata_Response *Peer_Metadata_Response_new (size_t size, char raw_response[2048], ssize_t res_size);
int Peer_Metadata_Response_init (Peer_Metadata_Response *this, char raw_response[2048], ssize_t res_size);
void Peer_Metadata_Response_destroy (Peer_Metadata_Response *this);
void Peer_Metadata_Response_print (Peer_Metadata_Response *this);

/* PEER HANDSHAKE WRAPPER */
typedef struct Peer_Metadata_Packet Peer_Metadata_Packet;
struct Peer_Metadata_Packet {
	int (*init) (Peer_Metadata_Packet *this, int piece_num, int ut_metadata);
    void (*print) (Peer_Metadata_Packet *this);
    void (*destroy) (Peer_Metadata_Packet *this);

	int (*send) (Peer_Metadata_Packet *this, Socket * socket);
	int (*receive) (Peer_Metadata_Packet *this, Socket * socket);

	Peer_Metadata_Request * request;
	Peer_Metadata_Response * response;
};

Peer_Metadata_Packet * Peer_Metadata_Packet_new (size_t size, int piece_num, int ut_metadata);
int Peer_Metadata_Packet_init (Peer_Metadata_Packet *this, int piece_num, int ut_metadata);
void Peer_Metadata_Packet_destroy (Peer_Metadata_Packet *this);
void Peer_Metadata_Packet_print (Peer_Metadata_Packet *this);

int Peer_Metadata_Packet_send (Peer_Metadata_Packet *this, Socket * socket);
int Peer_Metadata_Packet_receive (Peer_Metadata_Packet *this, Socket * socket);
#endif