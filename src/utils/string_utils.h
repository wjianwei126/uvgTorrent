#ifndef _stringutils_h
#define _stringutils_h

#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "hashmap/hashmap.h"


extern Hashmap * magnet_parse (const char * input);

static const struct
{
    Hashmap * (*magnet_parse) (const char * input);
} string_utils = {
    magnet_parse
};

#endif