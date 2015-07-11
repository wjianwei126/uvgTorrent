#include <string.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "macro/macro.h"
#include "debug/debug.h"

/**
* int hostname_to_ip(char * hostname , char* output)
*
* char    *hostname; hostname to lookup
* char    *output; pointer to copy ip insto
* 
* PURPOSE : look up a host and copy it's ip into the output pointer
* RETURN  : EXIT_SUCCESS || EXIT_FAILURE
*/
int hostname_to_ip(char * hostname , char* output)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;
         
    if ((he = gethostbyname(hostname)) == NULL) 
    {
        throw("DNS lookup failed");
    }
 
    addr_list = (struct in_addr **) he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) 
    {
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return EXIT_SUCCESS;
    }
     
    throw("no ip found");

error:
    return EXIT_FAILURE;
}