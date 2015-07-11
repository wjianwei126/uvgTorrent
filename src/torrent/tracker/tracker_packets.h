struct __attribute__ ((__packed__)) tracker_connect_request 
{

	uint64_t connection_id;
	uint32_t action;
	uint32_t transaction_id;
};

struct __attribute__ ((__packed__)) tracker_connect_response
{
	uint32_t action;
	uint32_t transaction_id;
	uint64_t connection_id;
};
/*
struct __attribute__ ((__packed__)) tracker_announce_request
{
	
};

struct __attribute__ ((__packed__)) tracker_announce_response
{
	
};

struct __attribute__ ((__packed__)) tracker_scrape_request
{
	
};

struct __attribute__ ((__packed__)) tracker_scrape_response
{
	
};
*/
struct __attribute__ ((__packed__)) tracker_error
{
	int32_t action;
	int32_t transaction_id;
	int8_t error_string[];
};