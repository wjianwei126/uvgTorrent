#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <math.h>
#include "utils/net/net_utils.h"
#include "peer/packets/peer_piece_packet.h"

/* CONNECT REQUEST */
Peer_Piece_Request * Peer_Piece_Request_new(size_t size, int piece_num)
{
	Peer_Piece_Request *req = malloc(size);
    check_mem(req);

    req->init = Peer_Piece_Request_init;
    req->destroy = Peer_Piece_Request_destroy;
    req->print= Peer_Piece_Request_print;

    if(req->init(req, piece_num) == EXIT_FAILURE) {
        throw("Peer_Piece_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Piece_Request_init(Peer_Piece_Request *this, int piece_num)
{
    char * bencoded_message = "d8:msg_typei0e5:piecei0ee";
    uint32_t length = net_utils.htonl(strlen(bencoded_message));
    uint8_t bt_msg_id = 20;
    uint8_t ut_metadata = 3;

    int pos = 0;
    memcpy(&this->bytes[pos], &length, sizeof(uint32_t));
    pos += sizeof(uint32_t);

    memcpy(&this->bytes[pos], &bt_msg_id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], &ut_metadata, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], bencoded_message, strlen(bencoded_message));
    pos += strlen(bencoded_message);

	return EXIT_SUCCESS;
}

void Peer_Piece_Request_destroy(Peer_Piece_Request *this)
{
	if(this) {
		free(this); 
	};
}

void Peer_Piece_Request_print(Peer_Piece_Request *this)
{

}

/* CONNECT RESPONSE */
Peer_Piece_Response * Peer_Piece_Response_new(size_t size, char raw_response[2048], ssize_t res_size)
{
	Peer_Piece_Response *req = malloc(size);
    check_mem(req);

    req->init = Peer_Piece_Response_init;
    req->destroy = Peer_Piece_Response_destroy;
    req->print= Peer_Piece_Response_print;

    if(req->init(req, raw_response, res_size) == EXIT_FAILURE) {
        throw("Peer_Piece_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Piece_Response_init(Peer_Piece_Response *this, char raw_response[2048], ssize_t res_size)
{
	char * bencoded_response = &raw_response[6];
	debug("piece 0 :: %s", bencoded_response);
    /*char * bencoded_response = &raw_response[6];
    char * metadata_key = "metadata_sizei";
    
    char * metadata_size_str = strstr(bencoded_response, metadata_key);
    int position =  metadata_size_str - bencoded_response;

    if(position < res_size){
        bencoded_response = bencoded_response + position + strlen(metadata_key);
        char buffer[10];

        int i;

        for (i = 0; i < 10; i++){
            if (bencoded_response[i] != 'e'){
                buffer[i] = bencoded_response[i];
            } else {
                buffer[i] = '\0';
                break;
            }
        }

        this->metadata_size = atoi(buffer);
        this->piece_size = 16*1024;
        this->num_pieces = this->metadata_size/this->piece_size;

    } else {
        this->metadata_size = 0;
    }*/

    return EXIT_SUCCESS;
}

void Peer_Piece_Response_destroy(Peer_Piece_Response *this)
{
	if(this) { 
		//if(this->pstr) { free(this->pstr); }
		//if(this->peer_id) { free(this->peer_id); }
		free(this); 
	};
}

void Peer_Piece_Response_print(Peer_Piece_Response *this)
{

}

/* CONNECT WRAPPER */
Peer_Piece_Packet * Peer_Piece_Packet_new (size_t size, int piece_num)
{
	Peer_Piece_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Peer_Piece_Packet_init;
    conn->destroy = Peer_Piece_Packet_destroy;
    conn->print = Peer_Piece_Packet_print;

    conn->send = Peer_Piece_Packet_send;
    conn->receive = Peer_Piece_Packet_receive;

    if(conn->init(conn, piece_num) == EXIT_FAILURE) {
        throw("Peer_Piece_Packet init failed");
    } else {
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Peer_Piece_Packet_init (Peer_Piece_Packet *this, int piece_num)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Peer_Piece_Request, piece_num);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Peer_Piece_Packet_destroy (Peer_Piece_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Peer_Piece_Packet_print (Peer_Piece_Packet *this)
{

}

int Peer_Piece_Packet_send (Peer_Piece_Packet *this, Socket * socket)
{
    return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Peer_Piece_Packet_receive (Peer_Piece_Packet *this, Socket * socket)
{
	char out[2048] = {0};
    ssize_t packet_size = socket->receive(socket, out, 2048);
	
    int chr;

    debug("%zu", packet_size);
    for(chr = 0; chr < packet_size; chr++){
    	printf("%c", out[chr]);
    }

    if(packet_size > 0){
    	/* prepare request */
		//this->response = NEW(Peer_Piece_Response, out, packet_size);

		//check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}