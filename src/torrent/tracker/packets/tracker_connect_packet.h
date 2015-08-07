#ifndef _tracker_connect_packet_h
#define _tracker_connect_packet_h

#include "macro/macro.h"
#include "data_structures/linkedlist/linkedlist.h"

/* TRACKER CONNECT REQUEST */
typedef struct Tracker_Connect_Request Tracker_Connect_Request;
struct Tracker_Connect_Request { 
	int (*init) (Tracker_Connect_Request *this, int32_t transaction_id);
    void (*print) (Tracker_Connect_Request *this);
    void (*destroy) (Tracker_Connect_Request *this);

	int64_t connection_id;		/* always 0x41727101980 */
	int32_t action;				/* always 0 			*/
	int32_t transaction_id;		/* always 0 			*/
	char bytes[16];
};

Tracker_Connect_Request *Tracker_Connect_Request_new (size_t size, int32_t transaction_id);
int Tracker_Connect_Request_init (Tracker_Connect_Request *this, int32_t transaction_id);
void Tracker_Connect_Request_destroy (Tracker_Connect_Request *this);
void Tracker_Connect_Request_print (Tracker_Connect_Request *this);

/* TRACKER CONNECT RESPONSE */
typedef struct Tracker_Connect_Response Tracker_Connect_Response;
struct Tracker_Connect_Response { 
	int (*init) (Tracker_Connect_Response *this, char raw_response[2048]);
    void (*print) (Tracker_Connect_Response *this);
    void (*destroy) (Tracker_Connect_Response *this);

	int32_t action; 			/* 0 == SUCCESS, 3 == ERROR 			*/
	int32_t transaction_id;		/* should match value sent with request */
	int64_t connection_id;		/* connection_id for future use 		*/
	Linkedlist * peers;
};

Tracker_Connect_Response *Tracker_Connect_Response_new (size_t size, char raw_response[2048]);
int Tracker_Connect_Response_init (Tracker_Connect_Response *this, char raw_response[2048]);
void Tracker_Connect_Response_destroy (Tracker_Connect_Response *this);
void Tracker_Connect_Response_print (Tracker_Connect_Response *this);

/* TRACKER CONNECT WRAPPER */
typedef struct Tracker_Connect Tracker_Connect;
struct Tracker_Connect {
	int (*init) (Tracker_Connect *this, int32_t transaction_id);
    void (*print) (Tracker_Connect *this);
    void (*destroy) (Tracker_Connect *this);

	int (*send) (Tracker_Connect *this, UDP_Socket * socket);
	ssize_t (*receive) (Tracker_Connect *this, UDP_Socket * socket);

	Tracker_Connect_Request * request;
	Tracker_Connect_Response * response;
};

Tracker_Connect * Tracker_Connect_new (size_t size, int32_t transaction_id);
int Tracker_Connect_init (Tracker_Connect *this, int32_t transaction_id);
void Tracker_Connect_destroy (Tracker_Connect *this);
void Tracker_Connect_print (Tracker_Connect *this);

int Tracker_Connect_send (Tracker_Connect *this, UDP_Socket * socket);
int Tracker_Connect_receive (Tracker_Connect *this, UDP_Socket * socket);
#endif