/**
* UDP tracker protocol
* http://www.libtorrent.org/udp_tracker_protocol.html
*/

struct __attribute__ ((__packed__)) tracker_connect_request 
{

	uint64_t connection_id;		/* connection id verified by tracker */
	uint32_t action; 			/* always 1 */
	uint32_t transaction_id;	/* randomly generated transaction id */
};

struct __attribute__ ((__packed__)) tracker_connect_response
{
	uint32_t action;
	uint32_t transaction_id;
	uint64_t connection_id;
};

struct __attribute__ ((__packed__)) tracker_announce_request
{
	/**
	* int64_t connection_id
	* 	connection id
	*/
	int64_t 	connection_id;
	int32_t 	action;
	int32_t 	transaction_id;
	int8_t	 	info_hash[20];
	int8_t	 	peer_id[20];
	int64_t 	downloaded;
	int64_t 	left;
	int64_t 	uploaded;

	/**
	* int32_t event
	*	none = 0
    *   completed = 1
    *   started = 2
    *   stopped = 3
	*/
	int32_t 	event;
	uint32_t 	ip;
	uint32_t 	key;
	int32_t 	num_want;
	uint16_t 	port;
	uint16_t 	extensions;
};

struct __attribute__ ((__packed__)) tracker_announce_response
{
	
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
	int32_t 	action;
	int32_t		transaction_id;
	int8_t		error_string[];
};