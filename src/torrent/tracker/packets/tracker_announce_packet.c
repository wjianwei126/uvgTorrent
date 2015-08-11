#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils/net/net_utils.h"
#include "torrent/tracker/packets/tracker_announce_packet.h"

/* CONNECT REQUEST */
Tracker_Announce_Request * Tracker_Announce_Request_new(size_t size, int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id)
{
	Tracker_Announce_Request *req = malloc(size);
    check_mem(req);

    req->init = Tracker_Announce_Request_init;
    req->destroy = Tracker_Announce_Request_destroy;
    req->print= Tracker_Announce_Request_print;

    if(req->init(req, connection_id, transaction_id, info_hash_bytes, peer_id) == EXIT_FAILURE) {
        throw("Tracker_Announce_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Tracker_Announce_Request_init(Tracker_Announce_Request *this, int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id)
{
	size_t length = 100;

	int32_t action = net_utils.htonl(1);
	int64_t downloaded = net_utils.htonll(0);
    int64_t left = net_utils.htonll(0);
    int64_t uploaded = net_utils.htonll(0);
    int32_t event = net_utils.htonl(0);
    uint32_t ip = net_utils.htonl(0);
    uint32_t key = net_utils.htonl(1);//rand_utils.nrand32(rand() % 10));
    int32_t num_want = net_utils.htonl(-1);
    uint16_t port = net_utils.htons(0);
    uint16_t extensions = net_utils.htons(0);

	memcpy(&this->connection_id, &connection_id, sizeof(int64_t));
	memcpy(&this->action, &action, sizeof(int32_t));
	memcpy(&this->transaction_id, &transaction_id, sizeof(int32_t));	
	memcpy(&this->info_hash, &info_hash_bytes, sizeof(int8_t) * 20);	
	memcpy(&this->peer_id, &peer_id, sizeof(int8_t) * 20);
	memcpy(&this->downloaded, &downloaded, sizeof(int64_t));
	memcpy(&this->left, &left, sizeof(int64_t));
	memcpy(&this->uploaded, &uploaded, sizeof(int64_t));
	memcpy(&this->event, &event, sizeof(int32_t));
	memcpy(&this->ip, &ip, sizeof(uint32_t));
	memcpy(&this->key, &key, sizeof(uint32_t));
	memcpy(&this->num_want, &num_want, sizeof(int32_t));
	memcpy(&this->port, &port, sizeof(uint16_t));
	memcpy(&this->extensions, &extensions, sizeof(uint16_t));


	size_t pos = 0;

	memcpy(&this->bytes[pos], &connection_id, 8);
	pos += sizeof(int64_t);

	memcpy(&this->bytes[pos], &action, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->bytes[pos], &transaction_id, sizeof(int32_t));
	pos += sizeof(int32_t);

    // convert 40 character info_hash stringlocated in magnet_uri to 20 byte array
    memcpy(&this->bytes[pos], info_hash_bytes, sizeof(int8_t) * 20);
    pos += sizeof(int8_t) * 20;

	memcpy(&this->bytes[pos], peer_id, sizeof(int8_t) * 20);
	pos += sizeof(int8_t) * 20;

	memcpy(&this->bytes[pos], &downloaded, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&this->bytes[pos], &left, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&this->bytes[pos], &uploaded, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&this->bytes[pos], &event, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->bytes[pos], &ip, sizeof(uint32_t));
	pos += sizeof(uint32_t);

	memcpy(&this->bytes[pos], &key, sizeof(uint32_t));
	pos += sizeof(uint32_t);

	memcpy(&this->bytes[pos], &num_want, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->bytes[pos], &port, sizeof(uint16_t));
	pos += sizeof(uint16_t);
	
	memcpy(&this->bytes[pos], &extensions, sizeof(uint16_t));
	pos += sizeof(uint16_t);

	return EXIT_SUCCESS;
}

void Tracker_Announce_Request_destroy(Tracker_Announce_Request *this)
{
	if(this) { free(this); };
}

void Tracker_Announce_Request_print(Tracker_Announce_Request *this)
{

}

/* CONNECT RESPONSE */
Tracker_Announce_Response * Tracker_Announce_Response_new(size_t size, char raw_response[2048])
{
	Tracker_Announce_Response *req = malloc(size);
    check_mem(req);

    req->init = Tracker_Announce_Response_init;
    req->destroy = Tracker_Announce_Response_destroy;
    req->print= Tracker_Announce_Response_print;

    if(req->init(req, raw_response) == EXIT_FAILURE) {
        throw("Tracker_Announce_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Tracker_Announce_Response_init(Tracker_Announce_Response *this, char raw_response[2048])
{
	/*
	size_t pos = 0;
	memcpy(&this->action, &raw_response[pos], sizeof(int32_t));
	this->action = net_utils.ntohl(this->action);
	pos += sizeof(int32_t);

	memcpy(&this->transaction_id, &raw_response[pos], sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->connection_id, &raw_response[pos], sizeof(int64_t));
	this->connection_id = net_utils.ntohll(this->connection_id);
	pos += sizeof(int64_t);*/

	return EXIT_SUCCESS;
}

void Tracker_Announce_Response_destroy(Tracker_Announce_Response *this)
{
	if(this) { free(this); };
}

void Tracker_Announce_Response_print(Tracker_Announce_Response *this)
{

}

/* CONNECT WRAPPER */
Tracker_Announce_Packet * Tracker_Announce_Packet_new (size_t size, int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id)
{
	Tracker_Announce_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Tracker_Announce_Packet_init;
    conn->destroy = Tracker_Announce_Packet_destroy;
    conn->print = Tracker_Announce_Packet_print;

    conn->send = Tracker_Announce_Packet_send;
    conn->receive = Tracker_Announce_Packet_receive;

    if(conn->init(conn, connection_id, transaction_id, info_hash_bytes, peer_id) == EXIT_FAILURE) {
        throw("Tracker_Announce_Packet init failed");
    } else {
        // all done, we made an object of any type
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Tracker_Announce_Packet_init (Tracker_Announce_Packet *this, int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Tracker_Announce_Request, connection_id, transaction_id, info_hash_bytes, peer_id);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Tracker_Announce_Packet_destroy (Tracker_Announce_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Tracker_Announce_Packet_print (Tracker_Announce_Packet *this)
{

}

int Tracker_Announce_Packet_send (Tracker_Announce_Packet *this, UDP_Socket * socket)
{
	return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Tracker_Announce_Packet_receive (Tracker_Announce_Packet *this, UDP_Socket * socket)
{
	char out[2048];
    ssize_t packet_size = socket->receive(socket, out);

    if(packet_size != -1){
    	/* prepare request */
		this->response = NEW(Tracker_Announce_Response, out);
		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}