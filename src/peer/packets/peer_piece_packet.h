#ifndef _peer_piece_packet_h
#define _peer_piece_packet_h

#include "macro/macro.h"
#include "utils/sock/socket.h"

/* PEER HANDSHAKE REQUEST */
typedef struct Peer_Piece_Request Peer_Piece_Request;
struct Peer_Piece_Request { 
	int (*init) (Peer_Piece_Request *this, int piece_num);
    void (*print) (Peer_Piece_Request *this);
    void (*destroy) (Peer_Piece_Request *this);

	int piece;
	char bytes[31];
};

Peer_Piece_Request *Peer_Piece_Request_new (size_t size, int piece_num);
int Peer_Piece_Request_init (Peer_Piece_Request *this, int piece_num);
void Peer_Piece_Request_destroy (Peer_Piece_Request *this);
void Peer_Piece_Request_print (Peer_Piece_Request *this);

/* PEER HANDSHAKE RESPONSE */
typedef struct Peer_Piece_Response Peer_Piece_Response;
struct Peer_Piece_Response { 
	int (*init) (Peer_Piece_Response *this, char raw_response[2048], ssize_t res_size);
    void (*print) (Peer_Piece_Response *this);
    void (*destroy) (Peer_Piece_Response *this);

	int8_t pstrlen;
	char * pstr;
	char * peer_id;
};

Peer_Piece_Response *Peer_Piece_Response_new (size_t size, char raw_response[2048], ssize_t res_size);
int Peer_Piece_Response_init (Peer_Piece_Response *this, char raw_response[2048], ssize_t res_size);
void Peer_Piece_Response_destroy (Peer_Piece_Response *this);
void Peer_Piece_Response_print (Peer_Piece_Response *this);

/* PEER HANDSHAKE WRAPPER */
typedef struct Peer_Piece_Packet Peer_Piece_Packet;
struct Peer_Piece_Packet {
	int (*init) (Peer_Piece_Packet *this, int piece_num);
    void (*print) (Peer_Piece_Packet *this);
    void (*destroy) (Peer_Piece_Packet *this);

	int (*send) (Peer_Piece_Packet *this, Socket * socket);
	int (*receive) (Peer_Piece_Packet *this, Socket * socket);

	Peer_Piece_Request * request;
	Peer_Piece_Response * response;
};

Peer_Piece_Packet * Peer_Piece_Packet_new (size_t size, int piece_num);
int Peer_Piece_Packet_init (Peer_Piece_Packet *this, int piece_num);
void Peer_Piece_Packet_destroy (Peer_Piece_Packet *this);
void Peer_Piece_Packet_print (Peer_Piece_Packet *this);

int Peer_Piece_Packet_send (Peer_Piece_Packet *this, Socket * socket);
int Peer_Piece_Packet_receive (Peer_Piece_Packet *this, Socket * socket);
#endif