#ifndef _peer_request_piece_packet_h
#define _peer_request_piece_packet_h

#include "macro/macro.h"
#include "utils/sock/socket.h"

/* PEER HANDSHAKE REQUEST */
typedef struct Peer_Request_Piece_Request Peer_Request_Piece_Request;
struct Peer_Request_Piece_Request { 
	int (*init) (Peer_Request_Piece_Request *this, int piece_num, int piece_size);
    void (*print) (Peer_Request_Piece_Request *this);
    void (*destroy) (Peer_Request_Piece_Request *this);

	int piece_num;
	int piece_size;
	char bytes[17];
};

Peer_Request_Piece_Request *Peer_Request_Piece_Request_new (size_t size, int piece_num, int piece_size);
int Peer_Request_Piece_Request_init (Peer_Request_Piece_Request *this, int piece_num, int piece_size);
void Peer_Request_Piece_Request_destroy (Peer_Request_Piece_Request *this);
void Peer_Request_Piece_Request_print (Peer_Request_Piece_Request *this);

/* PEER HANDSHAKE RESPONSE */
typedef struct Peer_Request_Piece_Response Peer_Request_Piece_Response;
struct Peer_Request_Piece_Response { 
	int (*init) (Peer_Request_Piece_Response *this, char raw_response[2048], ssize_t res_size);
    void (*print) (Peer_Request_Piece_Response *this);
    void (*destroy) (Peer_Request_Piece_Response *this);

	char * response;
	int response_len;
};

Peer_Request_Piece_Response *Peer_Request_Piece_Response_new (size_t size, char raw_response[2048], ssize_t res_size);
int Peer_Request_Piece_Response_init (Peer_Request_Piece_Response *this, char raw_response[2048], ssize_t res_size);
void Peer_Request_Piece_Response_destroy (Peer_Request_Piece_Response *this);
void Peer_Request_Piece_Response_print (Peer_Request_Piece_Response *this);

/* PEER HANDSHAKE WRAPPER */
typedef struct Peer_Request_Piece_Packet Peer_Request_Piece_Packet;
struct Peer_Request_Piece_Packet {
	int (*init) (Peer_Request_Piece_Packet *this, int piece_num, int piece_size, char * filepath);
    void (*print) (Peer_Request_Piece_Packet *this);
    void (*destroy) (Peer_Request_Piece_Packet *this);

	int (*send) (Peer_Request_Piece_Packet *this, Socket * socket);
	int (*receive) (Peer_Request_Piece_Packet *this, Socket * socket);

	Peer_Request_Piece_Request * request;
	Peer_Request_Piece_Response * response;
};

Peer_Request_Piece_Packet * Peer_Request_Piece_Packet_new (size_t size, int piece_num, int piece_size, char * filepath);
int Peer_Request_Piece_Packet_init (Peer_Request_Piece_Packet *this, int piece_num, int piece_size, char * filepath);
void Peer_Request_Piece_Packet_destroy (Peer_Request_Piece_Packet *this);
void Peer_Request_Piece_Packet_print (Peer_Request_Piece_Packet *this);

int Peer_Request_Piece_Packet_send (Peer_Request_Piece_Packet *this, Socket * socket);
int Peer_Request_Piece_Packet_receive (Peer_Request_Piece_Packet *this, Socket * socket);
#endif