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

    this->last_transaction_id = malloc(sizeof(int32_t));
    check_mem(this->last_transaction_id);

    this->connection_id = calloc(1,sizeof(int64_t));
    check_mem(this->connection_id);

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
        if(this->connection_id) { free(this->connection_id); }
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
    
    if(this->ip == NULL){
        // make dns request to get tracker ip
        this->ip = malloc(32);
        check_mem(this->ip);
        net_utils.hostname_to_ip(this->url, this->ip);
    }

    this->generate_transID(this);

    /* set up the packet to send to server */
    uint32_t transID = *this->last_transaction_id;
    if(transID == -1){
        throw("bad transID");
    }
    struct tracker_connect_request conn_request;
    conn_request.connection_id = htonll(0x41727101980); /* identifies protocol - don't change */
    conn_request.action = htonl(0);
    conn_request.transaction_id = transID; 
    
    if(this->tracker_socket == NULL) {
        /* set up our tracker socket connection */
        this->tracker_socket = NEW(UDP_Socket, this->ip, atoi(this->port));
        check_mem(this->tracker_socket);
    }

    int result = this->tracker_socket->connect(this->tracker_socket);
    if(result == EXIT_SUCCESS){
        // send packet
        this->tracker_socket->send(this->tracker_socket, &conn_request, sizeof(struct tracker_connect_request));

        void * out = malloc(2048);
        int conn_result = this->tracker_socket->receive(this->tracker_socket, out);
        if(conn_result == EXIT_SUCCESS){
            struct tracker_connect_response * resp = out;
            if(resp->action == 0 && *this->last_transaction_id == resp->transaction_id){
                this->connected = 1;
                this->attempts = 0;
                    
                memcpy(this->connection_id, &resp->connection_id, sizeof(int64_t));
                check_mem(this->connection_id);

                fprintf(stderr, " %s✔%s\n", KGRN, KNRM);
                debug("received connection_id from tracker :: %" PRId64, resp->connection_id);

                free(out);
                return EXIT_SUCCESS;
            } else {
                free(out);
                goto error;
            }
        } else {
            free(out);
            if(this->attempts < this->max_attempts){
                this->attempts++;
                this->connect(this);
            }
            goto error;
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
    int32_t transID = *this->last_transaction_id;
    
    struct tracker_announce_request conn_request;
    conn_request.connection_id = *this->connection_id; /* identifies protocol - don't change */
    conn_request.action = htonl(1);
    conn_request.transaction_id = transID;

    /* extract info hash */
    Linkedlist * info_hash_list = string_utils.split(torrent->hash, ':');
    const char * info_hash = (char *) info_hash_list->get(info_hash_list, 2);
    // convert 40 character info_hash stringlocated in magnet_uri to 20 byte array
    string_utils.hex_to_int8_t(info_hash, conn_request.info_hash, 40); // need to verify that tracker is receiving the correct value here
    
    // generate peer id
    for(int i = 0; i<=19; i++){
        conn_request.peer_id[i] = rand_utils.nrand8_t(rand() % 10);
    }
    
    conn_request.downloaded = htonll((uint64_t)0);
    conn_request.left = htonll((uint64_t)0);
    conn_request.uploaded = htonll((uint64_t)0);
    conn_request.event = htonl(0);
    conn_request.ip = htonl(0);
    conn_request.key = htonl(rand_utils.nrand32(rand() % 10));
    conn_request.num_want = htonl(-1);
    conn_request.port = htonl(5050);
    conn_request.extensions = htonl(0);
    // send packet
    this->tracker_socket->send(this->tracker_socket, &conn_request, sizeof(struct tracker_announce_request));

    void * out = malloc(2048);
    check_mem(out);
    int conn_result = this->tracker_socket->receive(this->tracker_socket, out);
    if(conn_result == EXIT_SUCCESS){
        struct tracker_error * resp = out;

        if(resp->action == 1 && *this->last_transaction_id == resp->transaction_id){
            struct tracker_announce_response * succ = out;
            
            this->attempts = 0;
            info_hash_list->destroy(info_hash_list);
            fprintf(stderr, " %s✔%s\n", KGRN, KNRM);

            debug("action :: %" PRId32, succ->action);
            debug("transaction_id :: %" PRId32, succ->transaction_id);
            debug("interval :: %d", (signed char)succ->interval);
            debug("leechers :: %d", (signed char)succ->leechers);
            debug("seeders :: %d", (signed char)succ->seeders);

            free(out);
            return EXIT_SUCCESS;
        } else {
            debug("action :: %" PRId32, resp->action);
            debug("transid1 :: %" PRId32, resp->transaction_id);
            debug("transid2 :: %" PRId32, *this->last_transaction_id);

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
    memcpy(this->last_transaction_id, &id, sizeof(int32_t));
}