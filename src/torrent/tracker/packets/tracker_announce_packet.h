#ifndef _Tracker_Announce_packet_h
#define _Tracker_Announce_packet_h

#include "macro/macro.h"
#include "utils/sock/udp_socket.h"
#include "data_structures/linkedlist/linkedlist.h"

/* TRACKER CONNECT REQUEST */
typedef struct Tracker_Announce_Request Tracker_Announce_Request;
struct Tracker_Announce_Request { 
	int (*init) (Tracker_Announce_Request *this, int32_t transaction_id);
    void (*print) (Tracker_Announce_Request *this);
    void (*destroy) (Tracker_Announce_Request *this);

	int64_t connection_id;		/* always 0x41727101980 */
	int32_t action;				/* always 0 			*/
	int32_t transaction_id;		/* always 0 			*/
	char bytes[16];
};

Tracker_Announce_Request *Tracker_Announce_Request_new (size_t size, int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id);
int Tracker_Announce_Request_init (Tracker_Announce_Request *this, int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id);
void Tracker_Announce_Request_destroy (Tracker_Announce_Request *this);
void Tracker_Announce_Request_print (Tracker_Announce_Request *this);

/* TRACKER CONNECT RESPONSE */
typedef struct Tracker_Announce_Response Tracker_Announce_Response;
struct Tracker_Announce_Response { 
	int (*init) (Tracker_Announce_Response *this, char raw_response[2048]);
    void (*print) (Tracker_Announce_Response *this);
    void (*destroy) (Tracker_Announce_Response *this);

	int32_t 	action; 		/* 1 == SUCCESS, 3 == ERROR */
	int32_t 	transaction_id; /* randomized transaction id */
	int32_t 	interval; 		/* number of seconds to wait before reannouncing */
	int32_t 	leechers;		/* number of leechers */
	int32_t 	seeders;		/* number of seeders */
	Linkedlist * peers;
};

Tracker_Announce_Response *Tracker_Announce_Response_new (size_t size, char raw_response[2048]);
int Tracker_Announce_Response_init (Tracker_Announce_Response *this, char raw_response[2048]);
void Tracker_Announce_Response_destroy (Tracker_Announce_Response *this);
void Tracker_Announce_Response_print (Tracker_Announce_Response *this);

/* TRACKER CONNECT WRAPPER */
typedef struct Tracker_Announce_Packet Tracker_Announce_Packet;
struct Tracker_Announce_Packet {
	int (*init) (Tracker_Announce_Packet *this, int32_t transaction_id);
    void (*print) (Tracker_Announce_Packet *this);
    void (*destroy) (Tracker_Announce_Packet *this);

	int (*send) (Tracker_Announce_Packet *this, UDP_Socket * socket);
	int (*receive) (Tracker_Announce_Packet *this, UDP_Socket * socket);

	Tracker_Announce_Request * request;
	Tracker_Announce_Response * response;
};

Tracker_Announce_Packet * Tracker_Announce_Packet_new (size_t size, int32_t transaction_id);
int Tracker_Announce_Packet_init (Tracker_Announce_Packet *this, int32_t transaction_id);
void Tracker_Announce_Packet_destroy (Tracker_Announce_Packet *this);
void Tracker_Announce_Packet_print (Tracker_Announce_Packet *this);

int Tracker_Announce_Packet_send (Tracker_Announce_Packet *this, UDP_Socket * socket);
int Tracker_Announce_Packet_receive (Tracker_Announce_Packet *this, UDP_Socket * socket);
#endif