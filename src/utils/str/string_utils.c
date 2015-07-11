#include "debug/debug.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "data_structures/hashmap/hashmap.h"
#include "data_structures/linkedlist/linkedlist.h"

/* inline functions */
/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character */
char to_hex(char code) {
  char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/**
* Linkedlist * string_split(char * input, char delimeter)
*
* char    *input; string to split
* char    delimeter;
* 
* PURPOSE : split a string, return linked list of sub strings
* RETURN  : Linkedlist * list of seperated strings
* NOTES   : Linkedlist * returned needs to be destroyed
*/
Linkedlist * string_split(char * input, char delimeter)
{
	Linkedlist * list = NEW(Linkedlist);
	char buffer[256];
  memset(buffer, 0, 256);
	int str_pos = 0;
	for (int i = 0; input[i] != '\0'; i++){
		if(input[i] != ':' && i < strlen(input)){
			buffer[str_pos] = input[i];
      str_pos++;
		} else {
      list->append(list, buffer, strlen(buffer));
      memset(buffer, 0, 256);
			str_pos = 0;
		}
	}
  list->append(list, buffer, strlen(buffer));

	return list;
}

/**
* char * string_urldecode(char * input)
*
* char    *input; string to decode
* 
* PURPOSE : decode a string
* RETURN  : url decoded string
*/
char * string_urldecode(char * input) 
{
  char *pstr = input, *buf = malloc(strlen(input) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') { 
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

/**
* char * string_urlencode(char * input)
*
* char    *input; string to encode
* 
* PURPOSE : encode a string
* RETURN  : url encoded string
*/
char * string_urlencode(char * input)
{
	char *pstr = input, *buf = malloc(strlen(input) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}