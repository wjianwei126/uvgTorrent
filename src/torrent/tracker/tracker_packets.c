#include "macro/macro.h"
#include "debug/debug.h"
#include "utils/str/string_utils.h"
#include "utils/net/net_utils.h"
#include "torrent/tracker/tracker_packets.h"
#include "data_structures/linkedlist/linkedlist.h"
#include <endian.h>
#include <inttypes.h>
#include <stdlib.h>

/* TRACKER CONNECT REQUEST */
int prepare_tracker_connect_request(int32_t transaction_id, char result[16]){
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

	assert(pos == length, "packet size error"); // assert that data fit into expected packet size

	return EXIT_SUCCESS;
error:
	return EXIT_FAILURE;
}

struct tracker_connect_response unpack_tracker_connect_response (char response[16])
{
	struct tracker_connect_response resp;

	size_t pos = 0;
	memcpy(&resp.action, &response[pos], sizeof(int32_t));
	resp.action = net_utils.ntohl(resp.action);
	pos += sizeof(int32_t);

	memcpy(&resp.transaction_id, &response[pos], sizeof(int32_t));
	pos += sizeof(int32_t);

	memcpy(&resp.connection_id, &response[pos], sizeof(int64_t));
	resp.connection_id = net_utils.ntohll(resp.connection_id);
	pos += sizeof(int64_t);

	return resp;
}


/* TRACKER ANNOUNCE REQUEST */
int prepare_tracker_announce_request(int64_t connection_id, int32_t transaction_id, int8_t info_hash_bytes[20], char *	peer_id, char result[100]){
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

	return EXIT_SUCCESS;
error:
	return EXIT_FAILURE;
}