#ifndef _randutils_h
#define _randutils_h

#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>

int32_t nrand32(int n);
int8_t nrand8_t(int n);

static const struct
{
    int32_t (*nrand32) (int n);
	int8_t (*nrand8_t) (int n);
} rand_utils = {
    nrand32,
    nrand8_t
};

#endif