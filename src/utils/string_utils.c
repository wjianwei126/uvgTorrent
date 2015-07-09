#include "debug/debug.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures/hashmap/hashmap.h"
#include "data_structures/linkedlist/linkedlist.h"

extern Linkedlist * string_split (char * input)
{
	Linkedlist * list = NEW(Linkedlist);

	return list;
}

extern char * string_urldecode (char * input)
{
	return input;
}

extern char * string_urlencode (char * input)
{
	return input;
}