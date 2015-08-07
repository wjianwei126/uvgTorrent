#include "utils/str/string_utils.h"
#include "utils/net/net_utils.h"
#include "data_structures/linkedlist/linkedlist.h"
#include <endian.h>
#include <inttypes.h>
#include <stdlib.h>
/**
* UDP tracker protocol
* http://www.libtorrent.org/udp_tracker_protocol.html
* http://www.bittorrent.org/beps/bep_0020.html
**/

/* TRACKER_CONNECT_REQUEST */


















/* TRACKER CONNECT REQUEST */
int prepare_tracker_connect_request(int32_t transaction_id, char result[16]);
struct tracker_connect_response unpack_tracker_connect_response (char response[16]);
struct tracker_connect_response
{
	int32_t action; 			/* 0 == SUCCESS, 3 == ERROR */
	int32_t transaction_id;		/* should match value sent with request */
	int64_t connection_id;		/* connection id verified by tracker */
};

static const struct
{
	int (*prepare_request) (int32_t transaction_id, char request[16]);
	struct tracker_connect_response (*unpack_response) (char response[16]);
} tracker_connect_request = {
	prepare_tracker_connect_request,
	unpack_tracker_connect_response
};

int prepare_tracker_announce_request(int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id, char result[100]);
struct tracker_announce_response unpack_tracker_announce_response (char response[2048]);
struct tracker_announce_response
{
	int32_t 	action; 		/* 1 == SUCCESS, 3 == ERROR */
	int32_t 	transaction_id; /* randomized transaction id */
	int32_t 	interval; 		/* number of seconds to wait before reannouncing */
	int32_t 	leechers;		/* number of leechers */
	int32_t 	seeders;		/* number of seeders */
	/**
	* The rest of the server reply is a variable number of the following structure:
	* int32_t 	ip;
	* uint16_t 	port;
	*/
};
static const struct
{
	int (*prepare_request)(int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char * peer_id, char result[98]);
	struct tracker_announce_response (*unpack_response) (char response[2048]);
} tracker_announce_request = {
	prepare_tracker_announce_request
};

/*
struct __attribute__ ((__packed__)) tracker_scrape_request
{
	
};

struct __attribute__ ((__packed__)) tracker_scrape_response
{
	
};
*/
struct __attribute__ ((__packed__)) tracker_error
{
	int32_t 	action; 		/* always 3 */
	int32_t		transaction_id; /* transaction id of messed up request */
	int8_t		error_string[]; /* printable error string */
};