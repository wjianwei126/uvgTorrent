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
    tracker->announce = Tracker_connect;

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

int Tracker_init(Tracker *this, char *address)
{
	this->port = NULL;

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
		free(this);
	}
}

void Tracker_print(Tracker *this)
{
	debug("Torrent Tracker :: %s:%s", this->url, this->port);
}

uint64_t nw_order(const uint64_t in) {
    unsigned char out[8] = {in>>56,in>>48,in>>40,in>>32,in>>24,in>>16,in>>8,in};
    return *(uint64_t*)out;
}

int Tracker_connect(Tracker *this)
{
    log_confirm("sending announce request :: %s:%s", this->url, this->port);

    struct tracker_connect_request conn_request;
    conn_request.connection_id = htonll(0x41727101980);
    conn_request.action = htonl(0);
    conn_request.transaction_id = htonl(112);
    UDP_Socket * udp = NEW(UDP_Socket, this->ip, atoi(this->port));
    //UDP_Socket * udp = NEW(UDP_Socket, "127.0.0.1", 20);
    int result = udp->connect(udp);
    if(result == EXIT_SUCCESS){
        // send packet
        udp->send(udp, &conn_request, 16);

        void * out = malloc(2048);
        udp->receive(udp, out);

        struct tracker_connect_response * test = out;
        log_info("%d", test->action);

        free(out);
        udp->destroy(udp);
    } else {
        throw("failed to connect to tracker");
    }

    fprintf(stderr, " %s✔%s\n", KGRN, KNRM);

    return EXIT_SUCCESS;

error: 
    if(udp){ udp->destroy(udp); };
    return EXIT_FAILURE;
}