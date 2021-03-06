#ifndef _netutils_h
#define _netutils_h

#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>


int hostname_to_ip (char * hostname , char* output);
uint64_t htonll_util (uint64_t input);
uint32_t htonl_util (uint32_t input);
uint16_t htons_util (uint16_t input);
uint64_t ntohll_util (uint64_t input);
uint32_t ntohl_util (uint32_t input);
uint16_t ntohs_util (uint16_t input);

static const struct
{
    int (*hostname_to_ip) (char * hostname , char* output);
    uint64_t (*htonll) (uint64_t input);
	uint32_t (*htonl) (uint32_t input);
	uint16_t (*htons) (uint16_t input);
	uint64_t (*ntohll) (uint64_t input);
	uint32_t (*ntohl) (uint32_t input);
	uint16_t (*ntohs) (uint16_t input);
} net_utils = {
    hostname_to_ip,
    htonll_util,
    htonl_util,
    htons_util,
    ntohll_util,
    ntohl_util,
    ntohs_util
};

#endif