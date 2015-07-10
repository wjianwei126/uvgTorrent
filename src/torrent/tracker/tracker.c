#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "utils/str/string_utils.h"
#include "utils/net/net_utils.h"
#include "utils/sock/udp_socket.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "torrent/tracker/tracker.h"

Tracker *Tracker_new(size_t size, char *address)
{
	Tracker *tracker = malloc(size);
    check_mem(tracker);

    tracker->init = Tracker_init;
    tracker->destroy = Tracker_destroy;
    tracker->print= Tracker_print;
    tracker->announce = Tracker_announce;

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

int Tracker_announce(Tracker *this)
{
    log_confirm("sending announce request :: %s:%s", this->url, this->port);

    char buf[2048];

    UDP_Socket * udp = NEW(UDP_Socket, this->ip, atoi(this->port));
    int result = udp->connect(udp);
    if(result == EXIT_SUCCESS){
        // send packet
        int i = 0;
        for (i=0; i < 2; i++) {
            //printf("Sending packet %d to %s port %d\n", i, this->url, atoi(this->port));
            sprintf(buf, "This is packet %d", i);
            if (sendto(*udp->sock_desc, buf, strlen(buf), 0, (const struct sockaddr *)udp->remote_addr, sizeof(*udp->remote_addr))==-1){
                throw("send failed");
            }
        }
        udp->destroy(udp);
    }

    fprintf(stderr, " %s✔%s\n", KGRN, KNRM);

    return EXIT_SUCCESS;

error: 
    if(udp){ udp->destroy(udp); };
    return EXIT_FAILURE;
}