#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "utils/net/net_utils.h"
#include "utils/bencode/bencode.h"
#include "peer/packets/peer_metadata_packet.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "data_structures/hashmap/hashmap.h"

/* CONNECT REQUEST */
Peer_Metadata_Request * Peer_Metadata_Request_new(size_t size, int piece_num, int ut_metadata)
{
	Peer_Metadata_Request *req = malloc(size);
    check_mem(req);

    req->init = Peer_Metadata_Request_init;
    req->destroy = Peer_Metadata_Request_destroy;
    req->print= Peer_Metadata_Request_print;

    if(req->init(req, piece_num, ut_metadata) == EXIT_FAILURE) {
        throw("peer_metadata_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Metadata_Request_init(Peer_Metadata_Request *this, int piece_num, int ut_metadata)
{
    char * bencoded_message_format = "d8:msg_typei0e5:piecei%iee\0";
    char bencoded_message[25];
    sprintf(bencoded_message, bencoded_message_format, piece_num);

    uint32_t length = net_utils.htonl(strlen(bencoded_message) + 2);
    uint8_t bt_msg_id = 20;
    uint8_t ut_metadata_uint = (uint8_t) ut_metadata;

    int pos = 0;
    memcpy(&this->bytes[pos], &length, sizeof(uint32_t));
    pos += sizeof(uint32_t);

    memcpy(&this->bytes[pos], &bt_msg_id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], &ut_metadata_uint, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], bencoded_message, strlen(bencoded_message));
    pos += strlen(bencoded_message);

	return EXIT_SUCCESS;
}

void Peer_Metadata_Request_destroy(Peer_Metadata_Request *this)
{
	if(this) {
		free(this); 
	};
}

void Peer_Metadata_Request_print(Peer_Metadata_Request *this)
{

}

/* CONNECT RESPONSE */
Peer_Metadata_Response * Peer_Metadata_Response_new(size_t size, char raw_response[2048], ssize_t res_size)
{
	Peer_Metadata_Response *req = malloc(size);
    check_mem(req);

    req->init = Peer_Metadata_Response_init;
    req->destroy = Peer_Metadata_Response_destroy;
    req->print= Peer_Metadata_Response_print;

    if(req->init(req, raw_response, res_size) == EXIT_FAILURE) {
        throw("peer_metadata_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Metadata_Response_init(Peer_Metadata_Response *this, char raw_response[2048], ssize_t res_size)
{
	uint32_t length;
    memcpy(&length, &raw_response[0], sizeof(uint32_t));
    length = net_utils.ntohl(length);
    length = length - 2;

    char bencoded_response[length+1];
    memcpy(&bencoded_response[0], &raw_response[6], length);
    bencoded_response[length] = '\0';

    char * find = "ee\0";
    char *result = strstr(&bencoded_response[0], find);
    int msg_pos = result - &bencoded_response[0];
    msg_pos += strlen(find);

    this->response = malloc(length + 1);
    memcpy(this->response, &bencoded_response[msg_pos], length - msg_pos);

    this->response_len = length;

    return EXIT_SUCCESS;
}

void Peer_Metadata_Response_destroy(Peer_Metadata_Response *this)
{
	if(this) {
		if(this->response) { free(this->response); }
		free(this); 
	};
}

void Peer_Metadata_Response_print(Peer_Metadata_Response *this)
{

}

/* CONNECT WRAPPER */
Peer_Metadata_Packet * Peer_Metadata_Packet_new (size_t size, int piece_num, int ut_metadata)
{
	Peer_Metadata_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Peer_Metadata_Packet_init;
    conn->destroy = Peer_Metadata_Packet_destroy;
    conn->print = Peer_Metadata_Packet_print;

    conn->send = Peer_Metadata_Packet_send;
    conn->receive = Peer_Metadata_Packet_receive;

    if(conn->init(conn, piece_num, ut_metadata) == EXIT_FAILURE) {
        throw("peer_metadata_Packet init failed");
    } else {
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Peer_Metadata_Packet_init (Peer_Metadata_Packet *this, int piece_num, int ut_metadata)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Peer_Metadata_Request, piece_num, ut_metadata);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Peer_Metadata_Packet_destroy (Peer_Metadata_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Peer_Metadata_Packet_print (Peer_Metadata_Packet *this)
{

}

int Peer_Metadata_Packet_send (Peer_Metadata_Packet *this, Socket * socket)
{
    return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Peer_Metadata_Packet_receive (Peer_Metadata_Packet *this, Socket * socket)
{
    // read packet length
	char out[17*2048] = {0};
    int pos = 0;
    int bytes_received = 0;

    socket->receive(socket, &out[pos], sizeof(uint32_t));
    uint32_t length;
    memcpy(&length, &out[pos], sizeof(uint32_t));
    length = net_utils.ntohl(length);
    pos += sizeof(uint32_t);

    while(bytes_received < length) {
        ssize_t packet_size = socket->receive(socket, &out[pos], length - bytes_received);
        bytes_received += packet_size;
        pos += packet_size;
    }

    if(bytes_received > 0){
    	/* prepare request */
		this->response = NEW(Peer_Metadata_Response, out, length + sizeof(uint32_t));

		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}