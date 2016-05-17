#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils/net/net_utils.h"
#include "peer/packets/peer_handshake_packet.h"

/* CONNECT REQUEST */
Peer_Handshake_Request * Peer_Handshake_Request_new(size_t size, char * info_hash, char * peer_id)
{
	Peer_Handshake_Request *req = malloc(size);
    check_mem(req);

    req->init = Peer_Handshake_Request_init;
    req->destroy = Peer_Handshake_Request_destroy;
    req->print= Peer_Handshake_Request_print;

    if(req->init(req, info_hash, peer_id) == EXIT_FAILURE) {
        throw("Peer_Handshake_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Handshake_Request_init(Peer_Handshake_Request *this, char * info_hash, char * peer_id)
{
	// size_t length = 16;

	/* store packet data in struct for easy debugging */
	char pstrlen = 19; //net_utils.htonl(19);
	char * pstr = "BitTorrent protocol";
	info_hash = "9609f0336566953f3bf342241b25e2437f65b2c8";

	int8_t info_hash_bytes[20];
    /* hex string to int8_t array */
    for(int count = 0; count < sizeof(info_hash_bytes); count++) {
        sscanf(info_hash, "%2hhx", &info_hash_bytes[count]);
        info_hash += 2 * sizeof(char);
    }

	this->pstr = malloc(strlen(pstr)+1);
    //check_mem(this->pstr);
	this->pstrlen = pstrlen;
	strcpy(this->pstr, pstr);

	/* store packet data in byte array for sending */
	size_t pos = 0;
	memcpy(&this->bytes[pos], &pstrlen, sizeof(char));
	pos += sizeof(char);

	memcpy(&this->bytes[pos], &pstr, strlen(pstr) * sizeof(int8_t));
	pos += strlen(pstr) * sizeof(int8_t);

	this->bytes[pos] = 0x0;
	this->bytes[pos+1] = 0x0;
	this->bytes[pos+2] = 0x0;
	this->bytes[pos+3] = 0x0;
	this->bytes[pos+4] = 0x0;
	this->bytes[pos+5] = 0x0;
	this->bytes[pos+6] = 0x0;
	this->bytes[pos+7] = 0x0;
	pos += 8;

	memcpy(&this->bytes[pos], &info_hash_bytes, 20 * sizeof(int8_t));
	pos += 20 * sizeof(int8_t);

	memcpy(&this->bytes[pos], &peer_id, strlen(peer_id) * sizeof(int8_t));
	pos += strlen(peer_id) * sizeof(int8_t);
	
	return EXIT_SUCCESS;
}

void Peer_Handshake_Request_destroy(Peer_Handshake_Request *this)
{
	if(this) { 
		if(this->pstr) { free(this->pstr); };

		free(this); 
	};
}

void Peer_Handshake_Request_print(Peer_Handshake_Request *this)
{

}

/* CONNECT RESPONSE */
Peer_Handshake_Response * Peer_Handshake_Response_new(size_t size, char raw_response[2048], ssize_t res_size)
{
	Peer_Handshake_Response *req = malloc(size);
    check_mem(req);

    req->init = Peer_Handshake_Response_init;
    req->destroy = Peer_Handshake_Response_destroy;
    req->print= Peer_Handshake_Response_print;

    if(req->init(req, raw_response, res_size) == EXIT_FAILURE) {
        throw("Peer_Handshake_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Handshake_Response_init(Peer_Handshake_Response *this, char raw_response[2048], ssize_t res_size)
{
	size_t pos = 0;

	memcpy(&this->pstrlen, &raw_response[pos], sizeof(int8_t));
	//this->pstrlen = net_utils.ntohs(this->pstrlen);
	pos += sizeof(int8_t);
	debug("PSTRLEN :: %i", this->pstrlen);

	/*
	char * pstr;
	char * info_hash;
	char * peer_id;
	
	memcpy(&this->action, &raw_response[pos], sizeof(int32_t));
	this->action = net_utils.ntohl(this->action);
	pos += sizeof(int32_t);

	memcpy(&this->transaction_id, &raw_response[pos], sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->connection_id, &raw_response[pos], sizeof(int64_t));
	this->connection_id = net_utils.ntohl(this->connection_id)*/

	return EXIT_SUCCESS;
}

void Peer_Handshake_Response_destroy(Peer_Handshake_Response *this)
{
	if(this) { free(this); };
}

void Peer_Handshake_Response_print(Peer_Handshake_Response *this)
{

}

/* CONNECT WRAPPER */
Peer_Handshake_Packet * Peer_Handshake_Packet_new (size_t size, char * info_hash, char * peer_id)
{
	Peer_Handshake_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Peer_Handshake_Packet_init;
    conn->destroy = Peer_Handshake_Packet_destroy;
    conn->print = Peer_Handshake_Packet_print;

    conn->send = Peer_Handshake_Packet_send;
    conn->receive = Peer_Handshake_Packet_receive;

    if(conn->init(conn, info_hash, peer_id) == EXIT_FAILURE) {
        throw("Peer_Handshake_Packet init failed");
    } else {
        // all done, we made an object of any type
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Peer_Handshake_Packet_init (Peer_Handshake_Packet *this, char * info_hash, char * peer_id)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Peer_Handshake_Request, info_hash, peer_id);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Peer_Handshake_Packet_destroy (Peer_Handshake_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Peer_Handshake_Packet_print (Peer_Handshake_Packet *this)
{

}

int Peer_Handshake_Packet_send (Peer_Handshake_Packet *this, Socket * socket)
{
	return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Peer_Handshake_Packet_receive (Peer_Handshake_Packet *this, Socket * socket)
{
	char out[2048];
    ssize_t packet_size = socket->receive(socket, out);

    debug("packet_size : %zd", packet_size);
    
    if(packet_size != -1){
    	/* prepare request */
		this->response = NEW(Peer_Handshake_Response, out, packet_size);

		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}