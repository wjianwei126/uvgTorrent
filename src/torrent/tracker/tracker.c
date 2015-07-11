#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <endian.h>
#include "utils/str/string_utils.h"
#include "utils/net/net_utils.h"
#include "utils/rand/rand_utils.h"
#include "utils/sock/udp_socket.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "torrent/tracker/tracker.h"
#include "torrent/tracker/tracker_packets.h"

Tracker *Tracker_new(size_t size, char *address)
{
	Tracker *tracker = malloc(size);
    check_mem(tracker);

    tracker->init = Tracker_init;
    tracker->destroy = Tracker_destroy;
    tracker->print= Tracker_print;
    
    tracker->connect = Tracker_connect;
    tracker->generate_transID = Tracker_generate_transID;

    if(tracker->init(tracker, address) == EXIT_FAILURE) {
        throw("tracker init failed");
    } else {
        // all done, we made an object of any type
        return tracker;
    }

error:
    if(tracker) { tracker->destroy(tracker); };
    return NULL;
}

/**
* int Tracker_init(Tracker *this, char *address)
*
* Tracker    *this; instance to initialize
* char       *address; address to tracker including protocol & port 
* 
* PURPOSE : pull host and port from url, use net_utils to get ip from host
* RETURN  : success bool
*/
int Tracker_init(Tracker *this, char *address)
{
	this->port = NULL;
    this->tracker_socket = NULL;
    this->last_transaction_id = 0;
    this->connected = 0;

	char * addr = string_utils.urldecode(address);

	Linkedlist * url_port = string_utils.split(addr, ':');
    const char * url = (char *) url_port->get(url_port, 1) + 2;
    const char * port = (char *) url_port->get(url_port, 2);

    this->port = malloc(strlen(port) + 1);
    check_mem(this->port);
    strcpy(this->port, port);

    this->url = calloc(1,strlen(url) + 1);
    check_mem(this->url);
    strcat(this->url, url);

    this->ip = malloc(32);
    net_utils.hostname_to_ip(this->url, this->ip);

    this->last_transaction_id = malloc(sizeof(uint32_t));
    check_mem(this->last_transaction_id);

    url_port->destroy(url_port);
    free(addr);
    
	return EXIT_SUCCESS;
error:
	return EXIT_FAILURE;
}

void Tracker_destroy(Tracker *this)
{
	if(this){
		if(this->port) { free(this->port); }
		if(this->url) { free(this->url); }
        if(this->ip) { free(this->ip); }
        if(this->last_transaction_id) { free(this->last_transaction_id); }
        if(this->tracker_socket){ this->tracker_socket->destroy(this->tracker_socket); }
		free(this);
	}
}

void Tracker_print(Tracker *this)
{
	debug("Torrent Tracker :: %s:%s", this->url, this->port);
}

/**
* int Tracker_connect(Tracker *this)
*
* Tracker    *this; instance to initialize
* 
* PURPOSE : send a connect request to the tracker, check if online
* RETURN  : success bool
* NOTES   : fails without throwing an error. the torrent which is
*           spawning will determine if all of the trackers have failed
*           and will throw and error if needed.
*/
int Tracker_connect(Tracker *this)
{
    log_confirm("sending connect request :: %s:%s", this->url, this->port);

    this->generate_transID(this);

    /* set up the packet to send to server */
    uint32_t transID = *this->last_transaction_id;
    struct tracker_connect_request conn_request;
    conn_request.connection_id = htonll(0x41727101980); /* identifies protocol - don't change */
    conn_request.action = htonl(0);
    conn_request.transaction_id = transID; 
    
    /* set up our tracker socket connection */
    this->tracker_socket = NEW(UDP_Socket, this->ip, atoi(this->port));
    check_mem(this->tracker_socket);

    int result = this->tracker_socket->connect(this->tracker_socket);
    if(result == EXIT_SUCCESS){
        // send packet
        this->tracker_socket->send(this->tracker_socket, &conn_request, 16);

        void * out = malloc(2048);
        int conn_result = this->tracker_socket->receive(this->tracker_socket, out);
        if(conn_result == EXIT_SUCCESS){
            this->connected = 1;
            struct tracker_connect_response * resp = out;
            if(resp->action == 0 && *this->last_transaction_id == resp->transaction_id){
                fprintf(stderr, " %s✔%s\n", KGRN, KNRM);

                free(out);
                return EXIT_SUCCESS;
            } else {
                free(out);
                goto error;
            }
        } else {
            free(out);
            goto error;
        }
    } else {
        throw("failed to connect to tracker");
    }

error: 
    fprintf(stderr, " %s✘%s\n", KRED, KNRM);
    return EXIT_FAILURE;
}

/**
* void Tracker_generate_transID(Tracker *this)
*
* Tracker    *this; instance to initialize
* 
* PURPOSE : generate a new random transaction_id
* NOTES   : needs to be called before each call to the tracker
*           response transaction_id must match request transaction_id
*/
void Tracker_generate_transID(Tracker *this)
{
    uint32_t id = htonl(rand_utils.nrand32(100));
    memcpy(this->last_transaction_id, &id, sizeof(uint32_t));
}