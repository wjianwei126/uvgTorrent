#include "utils/str/string_utils.h"
#include "utils/net/net_utils.h"
#include <endian.h>
#include <inttypes.h>
/**
* UDP tracker protocol
* http://www.libtorrent.org/udp_tracker_protocol.html
* http://www.bittorrent.org/beps/bep_0020.html
*
* structs are packed to ensure alignment regardless of environment
*/

/**
* These structs are used to make manipulating network data easierbut not for sending data
*
**/

void prepare_tracker_connect_request(int32_t transaction_id, char result[16]){
	size_t length = 16;

	int64_t connection_id = net_utils.htonll(0x41727101980);
	int32_t action = net_utils.htonl(0);

	size_t pos = 0;
	memcpy(&result[pos], &connection_id, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&result[pos], &action, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&result[pos], &transaction_id, sizeof(int32_t));
	pos += sizeof(int32_t);

	//assert(pos == length, "packet size error"); // assert that data fit into expected packet size

//error:
	//throw("packet size error");
}

static const struct
{
	void (*prepare)(int32_t transaction_id, char result[16]);
} tracker_connect_request = {
	prepare_tracker_connect_request
};

void prepare_tracker_announce_request(int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id, char result[100]){
	size_t length = 100;

	int32_t action = net_utils.htonl(1);
	int64_t downloaded = net_utils.htonll(0);
    int64_t left = net_utils.htonll(0);
    int64_t uploaded = net_utils.htonll(0);
    int32_t event = net_utils.htonl(0);
    uint32_t ip = net_utils.htonl(0);
    uint32_t key = net_utils.htonl(1);//rand_utils.nrand32(rand() % 10));
    int32_t num_want = net_utils.htonl(-1);
    uint16_t port = net_utils.htons(0);
    uint16_t extensions = net_utils.htons(0);
	
	int64_t conn_id = net_utils.htonll(connection_id);

	size_t pos = 0;
	memcpy(&result[pos], &conn_id, 8);
	pos += sizeof(int64_t);

	memcpy(&result[pos], &action, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&result[pos], &transaction_id, sizeof(int32_t));
	pos += sizeof(int32_t);

    // convert 40 character info_hash stringlocated in magnet_uri to 20 byte array
    memcpy(&result[pos], info_hash_bytes, 20);
    pos += 20;

	memcpy(&result[pos], peer_id, 20);
	pos += 20;

	memcpy(&result[pos], &downloaded, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&result[pos], &left, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&result[pos], &uploaded, sizeof(int64_t));
	pos += sizeof(int64_t);

	memcpy(&result[pos], &event, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&result[pos], &ip, sizeof(uint32_t));
	pos += sizeof(uint32_t);

	memcpy(&result[pos], &key, sizeof(uint32_t));
	pos += sizeof(uint32_t);

	memcpy(&result[pos], &num_want, sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&result[pos], &port, sizeof(uint16_t));
	pos += sizeof(uint16_t);
	
	memcpy(&result[pos], &extensions, sizeof(uint16_t));
	pos += sizeof(uint16_t);

	assert(pos == length, "packet size error"); // assert that data fit into expected packet size

error:
	throw("packet size error");
}

static const struct
{
	void (*prepare)(int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char * peer_id, char result[98]);
} tracker_announce_request = {
	prepare_tracker_announce_request
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