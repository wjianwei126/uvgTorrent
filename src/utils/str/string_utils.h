#ifndef _stringutils_h
#define _stringutils_h

#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <inttypes.h>
#include "data_structures/linkedlist/linkedlist.h"

Linkedlist * string_split (char * input, char delimeter);
char * string_urldecode (char * input);
int string_urlencode (char * input);
int8_t hex_to_int8_t(const char * str, int8_t * bytes, size_t blen);

static const struct
{
    Linkedlist * (*split) (char * input, char delimeter);
    char * (*urldecode) (char * input);
    int (*urlencode) (char * input);
    int8_t (*hex_to_int8_t) (const char * str, int8_t * bytes, size_t blen);
} string_utils = {
    string_split,
    string_urldecode,
    string_urlencode,
    hex_to_int8_t
};

#endif