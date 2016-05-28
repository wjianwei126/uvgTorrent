#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "utils/net/net_utils.h"
#include "tracker/packets/tracker_connect_packet.h"

/* CONNECT REQUEST */
Tracker_Connect_Request * Tracker_Connect_Request_new(size_t size, int32_t transaction_id)
{
	Tracker_Connect_Request *req = malloc(size);
    check_mem(req);

    req->init = Tracker_Connect_Request_init;
    req->destroy = Tracker_Connect_Request_destroy;
    req->print= Tracker_Connect_Request_print;

    if(req->init(req, transaction_id) == EXIT_FAILURE) {
        throw("Tracker_Connect_Request init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Tracker_Connect_Request_init(Tracker_Connect_Request *this, int32_t transaction_id)
{
	// size_t length = 16;

	/* store packet data in struct for easy debugging */
	int64_t connection_id = net_utils.htonll(0x41727101980);
	int32_t action = net_utils.htonl(0);

	memcpy(&this->connection_id, &connection_id, sizeof(int64_t));
	memcpy(&this->action, &action, sizeof(int32_t));
	memcpy(&this->transaction_id, &transaction_id, sizeof(int32_t));

	/* store packet data in byte array for sending */
	size_t pos = 0;
	memcpy(&this->bytes[pos], &connection_id, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&this->bytes[pos], &action, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->bytes[pos], &transaction_id, sizeof(int32_t));
	pos += sizeof(int32_t);

	return EXIT_SUCCESS;
}

void Tracker_Connect_Request_destroy(Tracker_Connect_Request *this)
{
	if(this) { free(this); };
}

void Tracker_Connect_Request_print(Tracker_Connect_Request *this)
{

}

/* CONNECT RESPONSE */
Tracker_Connect_Response * Tracker_Connect_Response_new(size_t size, char raw_response[2048], ssize_t res_size)
{
	Tracker_Connect_Response *req = malloc(size);
    check_mem(req);

    req->init = Tracker_Connect_Response_init;
    req->destroy = Tracker_Connect_Response_destroy;
    req->print= Tracker_Connect_Response_print;

    if(req->init(req, raw_response, res_size) == EXIT_FAILURE) {
        throw("Tracker_Connect_Response init failed");
    } else {
        // all done, we made an object of any type
        return req;
    }

error:
    if(req) { req->destroy(req); };
    return NULL;
}

int Tracker_Connect_Response_init(Tracker_Connect_Response *this, char raw_response[2048], ssize_t res_size)
{
	//struct tracker_connect_response resp;

	size_t pos = 0;
	memcpy(&this->action, &raw_response[pos], sizeof(int32_t));
	this->action = net_utils.ntohl(this->action);
	pos += sizeof(int32_t);

	memcpy(&this->transaction_id, &raw_response[pos], sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&this->connection_id, &raw_response[pos], sizeof(int64_t));
	this->connection_id = net_utils.ntohll(this->connection_id);
	pos += sizeof(int64_t);

	return EXIT_SUCCESS;
}

void Tracker_Connect_Response_destroy(Tracker_Connect_Response *this)
{
	if(this) { free(this); };
}

void Tracker_Connect_Response_print(Tracker_Connect_Response *this)
{

}

/* CONNECT WRAPPER */
Tracker_Connect_Packet * Tracker_Connect_Packet_new (size_t size, int32_t transaction_id)
{
	Tracker_Connect_Packet *conn = malloc(size);
    check_mem(conn);

    conn->init = Tracker_Connect_Packet_init;
    conn->destroy = Tracker_Connect_Packet_destroy;
    conn->print = Tracker_Connect_Packet_print;

    conn->send = Tracker_Connect_Packet_send;
    conn->receive = Tracker_Connect_Packet_receive;

    if(conn->init(conn, transaction_id) == EXIT_FAILURE) {
        throw("Tracker_Connect_Packet init failed");
    } else {
        // all done, we made an object of any type
        return conn;
    }

error:
    if(conn) { conn->destroy(conn); };
    return NULL;
}

int Tracker_Connect_Packet_init (Tracker_Connect_Packet *this, int32_t transaction_id)
{
	this->request = NULL;
	this->response = NULL;

	/* prepare request */
	this->request = NEW(Tracker_Connect_Request, transaction_id);
	check_mem(this->request);

	return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

void Tracker_Connect_Packet_destroy (Tracker_Connect_Packet *this)
{
	if(this){
		if(this->request != NULL) { this->request->destroy(this->request); };
		if(this->response != NULL) { this->response->destroy(this->response); };
		free(this);
	}
}

void Tracker_Connect_Packet_print (Tracker_Connect_Packet *this)
{

}

int Tracker_Connect_Packet_send (Tracker_Connect_Packet *this, Socket * socket)
{
	return socket->send(socket, &this->request->bytes, sizeof(this->request->bytes));
}

int Tracker_Connect_Packet_receive (Tracker_Connect_Packet *this, Socket * socket)
{
	char out[2048];
    ssize_t packet_size = socket->receive(socket, out, 2048);

    if(packet_size != -1){
    	/* prepare request */
		this->response = NEW(Tracker_Connect_Response, out, packet_size);
		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}