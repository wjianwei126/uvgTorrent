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
#include <inttypes.h>
#include "utils/str/string_utils.h"
#include "utils/net/net_utils.h"
#include "utils/rand/rand_utils.h"
#include "utils/sock/udp_socket.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "torrent/tracker/tracker.h"
#include "torrent/tracker/tracker_packets.h"
#include "torrent/torrent.h"

Tracker *Tracker_new(size_t size, char *address)
{
	Tracker *tracker = malloc(size);
    check_mem(tracker);

    tracker->init = Tracker_init;
    tracker->destroy = Tracker_destroy;
    tracker->print= Tracker_print;
    
    tracker->connect = Tracker_connect;
    tracker->announce = Tracker_announce;
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
    this->ip = NULL;
	this->port = NULL;
    this->tracker_socket = NULL;
    this->connection_id = 0;
    this->last_transaction_id = 0;
    this->connected = 0;
    this->attempts = 0;
    this->max_attempts = 5;

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

    // make dns request to get tracker ip
    this->ip = malloc(32);
    check_mem(this->ip);
    net_utils.hostname_to_ip(this->url, this->ip);

    this->last_transaction_id = 0;
    this->connection_id = 0;

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
* Tracker    *this;
* 
* PURPOSE : send a connect request to the tracker, check if online
* RETURN  : success bool
* NOTES   : fails without throwing an error. the torrent which is
*           spawning will determine if all of the trackers have failed
*           and will throw and error if needed.
*/
int Tracker_connect(Tracker *this)
{
    if(this->attempts == 0){
        log_confirm("sending connect request :: %s:%s", this->url, this->port);
    }

    /* set up the packet to send to server */
    this->generate_transID(this);
    assert(this->last_transaction_id == -1, "bad transID")

    char conn_request[16];
    tracker_connect_request.prepare(this->last_transaction_id, conn_request);
    
    if(this->tracker_socket == NULL) {
        /* set up our tracker socket connection */
        this->tracker_socket = NEW(UDP_Socket, this->ip, atoi(this->port));
        check_mem(this->tracker_socket);
    }

    int result = this->tracker_socket->connect(this->tracker_socket);
    if(result == EXIT_SUCCESS){
        // send packet
        this->tracker_socket->send(this->tracker_socket, &conn_request, sizeof(conn_request));

        signed char out[2048];
        int conn_result = this->tracker_socket->receive(this->tracker_socket, out);
        if(conn_result == EXIT_SUCCESS){
            int32_t action;
            int32_t transaction_id;

            memcpy(&action, &out[0], sizeof(int32_t));
            memcpy(&transaction_id, &out[4], sizeof(int32_t));

            if(action == 0 && this->last_transaction_id == transaction_id){
                this->connected = 1;
                this->attempts = 0;

                int64_t connection_id;
                memcpy(&connection_id, &out[8], sizeof(int64_t));
                connection_id = net_utils.ntohll(connection_id);
                memcpy(&this->connection_id, &connection_id, sizeof(int64_t));
                
                fprintf(stderr, " %s✔%s\n", KGRN, KNRM);
                debug("received connection_id %" PRId64, this->connection_id);
                
                return EXIT_SUCCESS;
            } else {
                goto error;
            }
        } else {
            if(this->attempts < this->max_attempts){
                this->attempts++;
                return this->connect(this);
            } else {
                goto error;
            }
        }
    } else {
        throw("failed to connect to tracker");
    }

    return EXIT_SUCCESS;

error:
    if(this->attempts != 0){
        this->attempts = 0;
        fprintf(stderr, " %s✘%s\n", KRED, KNRM);
    }
    return EXIT_FAILURE;
}

/**
* int Tracker_announce(Tracker *this)
*
* Tracker    *this; instance to initialize
* 
* PURPOSE : send an announce request to the tracker, retreive peer information
* RETURN  : success bool
* NOTES   : fails without throwing an error.
*/
int Tracker_announce(Tracker *this, Torrent *torrent)
{
    if(this->connected == 0){
        return EXIT_SUCCESS;
    }

    if(this->attempts == 0){
        log_confirm("sending announce request :: %s:%s", this->url, this->port);
    }

    this->generate_transID(this);

    /* set up the packet to send to server */
    int32_t transID = this->last_transaction_id;
    /* extract info hash */
    Linkedlist * info_hash_list = string_utils.split(torrent->hash, ':');
    const char * info_hash = (char *) info_hash_list->get(info_hash_list, 2);
    
    int8_t info_hash_bytes[20];
    /* hex string to int8_t array */
    for(int count = 0; count < sizeof(info_hash_bytes)/sizeof(info_hash_bytes[0]); count++) {
        sscanf(info_hash, "%2hhx", &info_hash_bytes[count]);
        info_hash += 2 * sizeof(char);
    }

    /* hardcoded peer_id -> replace with random val */
    char * peer_id = "UVG01234567891234567";

    /* prepare the announce request packet */
    char conn_request[100];
    tracker_announce_request.prepare(this->connection_id, transID, info_hash_bytes, peer_id, conn_request);
    
    // send packet
    this->tracker_socket->send(this->tracker_socket, &conn_request, sizeof(conn_request));

    void * out = malloc(2048);
    check_mem(out);
    int conn_result = this->tracker_socket->receive(this->tracker_socket, out);
    if(conn_result == EXIT_SUCCESS){
        struct tracker_error * resp = out;
        resp->action = ntohl(resp->action);
        if(resp->action == 1 && this->last_transaction_id == resp->transaction_id){
            struct tracker_announce_response * succ = out;
            
            succ->interval = ntohl(succ->interval);
            succ->leechers = ntohl(succ->leechers);
            succ->seeders = ntohl(succ->seeders);

            this->attempts = 0;
            info_hash_list->destroy(info_hash_list);
            fprintf(stderr, " %s✔%s\n", KGRN, KNRM);

            debug("action :: %" PRId32, succ->action);
            debug("transaction_id :: %" PRId32, succ->transaction_id);
            debug("interval :: %" PRId32, succ->interval);
            debug("leechers :: %" PRId32, succ->leechers);
            debug("seeders :: %" PRId32, succ->seeders);

            free(out);
            return EXIT_SUCCESS;
        } else {
            debug("action :: %" PRId32, resp->action);
            debug("transid1 :: %" PRId32, resp->transaction_id);
            debug("transid2 :: %" PRId32, this->last_transaction_id);

            fprintf(stderr, " %s✘%s\n", KRED, KNRM);
            fprintf(stderr, KRED "[ERROR] %s\n" KNRM, ((strlen((char *)resp->error_string) != 0) ? (char *)resp->error_string : "no error given by tracker"));
            free(out);
            goto error;
        }
    } else {
        if(this->attempts < this->max_attempts){
            this->attempts++;
            this->announce(this, torrent);
        }
    }

    info_hash_list->destroy(info_hash_list);
    free(out);
    return EXIT_SUCCESS;
error:
    if(this->attempts != 0){
        this->attempts = 0;
        fprintf(stderr, " %s✘%s\n", KRED, KNRM);
    }
    if(info_hash_list != NULL) { info_hash_list->destroy(info_hash_list); };
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
    int seed = rand() % 10;

    int32_t id = htonl(rand_utils.nrand32(seed));
    memcpy(&this->last_transaction_id, &id, sizeof(int32_t));
}