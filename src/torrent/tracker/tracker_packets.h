/**
* UDP tracker protocol
* http://www.libtorrent.org/udp_tracker_protocol.html
*
* structs are packed to ensure alignment regardless of environment
*/

struct __attribute__ ((__packed__)) tracker_connect_request 
{

	uint64_t connection_id;		/* connection id verified by tracker */
	uint32_t action; 			/* always 0 */
	uint32_t transaction_id;	/* randomly generated transaction id */
};

struct __attribute__ ((__packed__)) tracker_connect_response
{
	uint32_t action; 			/* 0 == SUCCESS, 3 == ERROR */
	uint32_t transaction_id;	/* should match value sent with request */
	uint64_t connection_id;		/* connection id verified by tracker */
};

struct __attribute__ ((__packed__)) tracker_announce_request
{
	int64_t 	connection_id; 	/* connection id verified by tracker */
	int32_t 	action; 		/* always 1 */
	int32_t 	transaction_id; /* randomly generated transaction id */
	int8_t	 	info_hash[20];	/* torrent info hash */
	int8_t	 	peer_id[20];	/* generated peer id */
	int64_t 	downloaded;		/* number of bytes downloaded */
	int64_t 	left;			/* number of bytes remaining */
	int64_t 	uploaded;		/* number of bytes uploaded */

	
	int32_t 	event;			/* event being announced 
								*	none = 0
							    *   completed = 1
							    *   started = 2
							    *   stopped = 3 */

	uint32_t 	ip;				/* local ip address */
	uint32_t 	key;			/* randomized unique key */
	int32_t 	num_want;		/* maximum number of desired peers - -1 default */
	uint16_t 	port;			/* local port */
	uint16_t 	extensions;
};

struct __attribute__ ((__packed__)) tracker_announce_response
{
	int32_t 	action; 		/* 1 == SUCCESS, 3 == ERROR */
	int32_t 	transaction_id; /* randomized transaction id */
	int32_t 	interval; 		/* number of seconds to wait before reannouncing */
	int32_t 	leechers;		/* number of leechers */
	int32_t 	seeders;		/* number of seeders */

	/**
	* The rest of the server reply is a variable number of the following structure:
	* int32_t 	ip 	
	* uint16_t 	port
	*/
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