#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include "utils/net/net_utils.h"
#include "peer/packets/peer_extended_handshake_packet.h"

/* CONNECT REQUEST */
Peer_Extended_Handshake_Request * Peer_Extended_Handshake_Request_new(size_t size)
{
	Peer_Extended_Handshake_Request *req = malloc(size);
    check_mem(req);

    req->init = Peer_Extended_Handshake_Request_init;
    req->destroy = Peer_Extended_Handshake_Request_destroy;
    req->print= Peer_Extended_Handshake_Request_print;

    if(req->init(req) == EXIT_FAILURE) {
        throw("Peer_Extended_Handshake_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Extended_Handshake_Request_init(Peer_Extended_Handshake_Request *this)
{
    char * bencoded_message = "d1:md11:ut_metadatai1eee";
    
    uint32_t length = net_utils.htonl(26);
    uint8_t bt_msg_id = 20;
    uint8_t handshake_id = 0; 

    int pos = 0;
    memcpy(&this->bytes[pos], &length, sizeof(uint32_t));
    pos += sizeof(uint32_t);

    memcpy(&this->bytes[pos], &bt_msg_id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], &handshake_id, sizeof(uint8_t));
    pos += sizeof(uint8_t);

    memcpy(&this->bytes[pos], bencoded_message, strlen(bencoded_message));
    pos += strlen(bencoded_message);

	return EXIT_SUCCESS;
}

void Peer_Extended_Handshake_Request_destroy(Peer_Extended_Handshake_Request *this)
{
	if(this) {
		free(this); 
	};
}

void Peer_Extended_Handshake_Request_print(Peer_Extended_Handshake_Request *this)
{

}

/* CONNECT RESPONSE */
Peer_Extended_Handshake_Response * Peer_Extended_Handshake_Response_new(size_t size, char raw_response[2048], ssize_t res_size)
{
	Peer_Extended_Handshake_Response *req = malloc(size);
    check_mem(req);

    req->init = Peer_Extended_Handshake_Response_init;
    req->destroy = Peer_Extended_Handshake_Response_destroy;
    req->print= Peer_Extended_Handshake_Response_print;

    if(req->init(req, raw_response, res_size) == EXIT_FAILURE) {
        throw("Peer_Extended_Handshake_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Peer_Extended_Handshake_Response_init(Peer_Extended_Handshake_Response *this, char raw_response[2048], ssize_t res_size)
{
    char * bencoded_response = &raw_response[6];
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
    } else {
        this->metadata_size = 0;
    }

    return EXIT_SUCCESS;
}

void Peer_Extended_Handshake_Response_destroy(Peer_Extended_Handshake_Response *this)
{
	if(this) { 
		//if(this->pstr) { free(this->pstr); }
		//if(this->peer_id) { free(this->peer_id); }
		free(this); 
	};
}

void Peer_Extended_Handshake_Response_print(Peer_Extended_Handshake_Response *this)
{

}

/* CONNECT WRAPPER */
Peer_Extended_Handshake_Packet * Peer_Extended_Handshake_Packet_new (size_t size)
{
	Peer_Extended_Handshake_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Peer_Extended_Handshake_Packet_init;
    conn->destroy = Peer_Extended_Handshake_Packet_destroy;
    conn->print = Peer_Extended_Handshake_Packet_print;

    conn->send = Peer_Extended_Handshake_Packet_send;
    conn->receive = Peer_Extended_Handshake_Packet_receive;

    if(conn->init(conn) == EXIT_FAILURE) {
        throw("Peer_Extended_Handshake_Packet init failed");
    } else {
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Peer_Extended_Handshake_Packet_init (Peer_Extended_Handshake_Packet *this)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Peer_Extended_Handshake_Request);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Peer_Extended_Handshake_Packet_destroy (Peer_Extended_Handshake_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Peer_Extended_Handshake_Packet_print (Peer_Extended_Handshake_Packet *this)
{

}

int Peer_Extended_Handshake_Packet_send (Peer_Extended_Handshake_Packet *this, Socket * socket)
{
	return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Peer_Extended_Handshake_Packet_receive (Peer_Extended_Handshake_Packet *this, Socket * socket)
{
	char out[2048];
    ssize_t packet_size = socket->receive(socket, out, 200);
    
    if(packet_size > 0){
    	/* prepare request */
		this->response = NEW(Peer_Extended_Handshake_Response, out, packet_size);

		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}