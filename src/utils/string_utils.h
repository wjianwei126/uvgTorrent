#ifndef _stringutils_h
#define _stringutils_h

#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include "data_structures/linkedlist/linkedlist.h"

extern Linkedlist * string_split (char * input);
extern char * string_urldecode (char * input);
extern char * string_urlencode (char * input);

static const struct
{
    Linkedlist * (*split) (char * input);
    char * (*urldecode) (char * input);
    char * (*urlencode) (char * input);
} string_utils = {
    string_split,
    string_urldecode,
    string_urlencode
};

#endif