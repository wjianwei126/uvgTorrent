#ifndef _netutils_h
#define _netutils_h

#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>


int hostname_to_ip(char * hostname , char* ip);

static const struct
{
    int (*hostname_to_ip) (char * hostname , char* ip);
} net_utils = {
    hostname_to_ip
};

#endif