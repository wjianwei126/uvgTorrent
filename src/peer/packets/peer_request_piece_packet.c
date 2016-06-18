#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils/net/net_utils.h"
#include "utils/bencode/bencode.h"
#include "peer/packets/peer_request_piece_packet.h"

/* CONNECT REQUEST */
Peer_Request_Piece_Request * Peer_Request_Piece_Request_new(size_t size, int piece_num, int piece_size)
{
	Peer_Request_Piece_Request *req = malloc(size);
    check_mem(req);

    req->init = Peer_Request_Piece_Request_init;
    req->destroy = Peer_Request_Piece_Request_destroy;
    req->print= Peer_Request_Piece_Request_print;

    if(req->init(req, piece_num, piece_size) == EXIT_FAILURE) {
        throw("Peer_Request_Piece_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Request_Piece_Request_init(Peer_Request_Piece_Request *this, int piece_num, int piece_size)
{
    uint32_t msg_length = net_utils.htonl(13);
    uint8_t id = 6;
    uint32_t index = 0;
    uint32_t begin = 0;
    uint32_t piece_length = 32;

    int pos = 0;

    memcpy(&this->bytes[pos], &msg_length, sizeof(uint32_t));
    pos += sizeof(uint32_t);

    memcpy(&this->bytes[pos], &id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], &index, sizeof(uint32_t));
    pos += sizeof(uint32_t);

    memcpy(&this->bytes[pos], &begin, sizeof(uint32_t));
    pos += sizeof(uint32_t);

    memcpy(&this->bytes[pos], &piece_length, sizeof(uint32_t));
    pos += sizeof(uint32_t);

	return EXIT_SUCCESS;
}

void Peer_Request_Piece_Request_destroy(Peer_Request_Piece_Request *this)
{
	if(this) {
		free(this); 
	};
}

void Peer_Request_Piece_Request_print(Peer_Request_Piece_Request *this)
{

}

/* CONNECT RESPONSE */
Peer_Request_Piece_Response * Peer_Request_Piece_Response_new(size_t size, char raw_response[2048], ssize_t res_size)
{
	Peer_Request_Piece_Response *req = malloc(size);
    check_mem(req);

    req->init = Peer_Request_Piece_Response_init;
    req->destroy = Peer_Request_Piece_Response_destroy;
    req->print= Peer_Request_Piece_Response_print;

    if(req->init(req, raw_response, res_size) == EXIT_FAILURE) {
        throw("Peer_Request_Piece_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Request_Piece_Response_init(Peer_Request_Piece_Response *this, char raw_response[2048], ssize_t res_size)
{
    this->response = NULL;
    return EXIT_SUCCESS;
}

void Peer_Request_Piece_Response_destroy(Peer_Request_Piece_Response *this)
{
	if(this) {
		if(this->response) { free(this->response); }
		free(this); 
	};
}

void Peer_Request_Piece_Response_print(Peer_Request_Piece_Response *this)
{

}

/* CONNECT WRAPPER */
Peer_Request_Piece_Packet * Peer_Request_Piece_Packet_new (size_t size, int piece_num, int piece_size, char * filepath)
{
	Peer_Request_Piece_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Peer_Request_Piece_Packet_init;
    conn->destroy = Peer_Request_Piece_Packet_destroy;
    conn->print = Peer_Request_Piece_Packet_print;

    conn->send = Peer_Request_Piece_Packet_send;
    conn->receive = Peer_Request_Piece_Packet_receive;

    if(conn->init(conn, piece_num, piece_size, filepath) == EXIT_FAILURE) {
        throw("Peer_Request_Piece_Packet init failed");
    } else {
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Peer_Request_Piece_Packet_init (Peer_Request_Piece_Packet *this, int piece_num, int piece_size, char * filepath)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Peer_Request_Piece_Request, piece_num, piece_size);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Peer_Request_Piece_Packet_destroy (Peer_Request_Piece_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Peer_Request_Piece_Packet_print (Peer_Request_Piece_Packet *this)
{

}

int Peer_Request_Piece_Packet_send (Peer_Request_Piece_Packet *this, Socket * socket)
{
    return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Peer_Request_Piece_Packet_receive (Peer_Request_Piece_Packet *this, Socket * socket)
{
	char out[17*2048] = {0};
    int pos = 0;
    int bytes_received = 0;

    socket->receive(socket, &out[pos], sizeof(uint32_t));
    uint32_t length;
    memcpy(&length, &out[pos], sizeof(uint32_t));
    length = net_utils.ntohl(length);
    pos += sizeof(uint32_t);

    debug("%" PRId32, length);

    while(bytes_received < length) {
        ssize_t packet_size = socket->receive(socket, &out[pos], length - bytes_received);
        bytes_received += packet_size;
        pos += packet_size;
    }

    if(bytes_received > 0){
    	/* prepare request */
		this->response = NEW(Peer_Request_Piece_Response, out, length + sizeof(uint32_t));

		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}