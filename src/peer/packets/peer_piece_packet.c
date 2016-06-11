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
#include "peer/packets/peer_piece_packet.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "data_structures/hashmap/hashmap.h"

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
    char * bencoded_message_format = "d8:msg_typei0e5:piecei%iee\0";
    char bencoded_message[25];
    sprintf(bencoded_message, bencoded_message_format, piece_num);

    uint32_t length = net_utils.htonl(strlen(bencoded_message) + 2);
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
	uint32_t length;
    memcpy(&length, &raw_response[0], sizeof(uint32_t));
    length = net_utils.ntohl(length);
    length = length - 2;

    char bencoded_response[length];
    memcpy(&bencoded_response[0], &raw_response[6], length);
    
    char * test = "d5:filesld6:lengthi439837573e4:pathl44:Game.of.Thrones.S05E10.HDTV.x264-KILLERS.mp4eed6:lengthi172e4:pathl43:Torrent-Downloaded-From-extratorrent.cc.txteed6:lengthi10869e4:pathl44:game.of.thrones.s05e10.hdtv.x264-killers.nfoeee4:name46:Game.of.Thrones.S05E10.HDTV.x264-KILLERS[ettv]12:piece lengthi262144e\0";
    

    bencode_t * bencoded = malloc(sizeof(bencode_t));
    bencode_init(
        bencoded,
        &test[0],
        strlen(test));
    
    Hashmap * bencoded_hashmap = bencode_to_hashmap(bencoded);
    const Linkedlist * files = bencoded_hashmap->get(bencoded_hashmap, "files");
    Linkednode * curr = files->head;

    while(curr){
        Hashmap * file = (Hashmap *)curr->get(curr);

        int * length = file->get(file, "length");

        Linkedlist * path = file->get(file, "path");

        Linkednode * path_curr = path->head;
        char * path_str;
        while(path_curr){
            path_str = path_curr->get(path_curr);
            path_curr = path_curr->next;
        }

        debug("FILE FOUND :: %s SIZE :: %i", path_str, *length);

        curr = curr->next;
    }
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
    return socket->send(socket, &this->request->bytes, 31);//sizeof(this->request->bytes));
}

int Peer_Piece_Packet_receive (Peer_Piece_Packet *this, Socket * socket)
{
	char out[17*2048] = {0};
    ssize_t packet_size = socket->receive(socket, out, 17*2048);

    if(packet_size > 0){
    	/* prepare request */
		this->response = NEW(Peer_Piece_Response, out, packet_size);

		check_mem(this->response);

    	return EXIT_SUCCESS;
    } else {
    	return EXIT_FAILURE;
    }
error:
	return EXIT_FAILURE;
}