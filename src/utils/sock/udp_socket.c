#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include "utils/sock/udp_socket.h"
#include "utils/str/string_utils.h"

UDP_Socket *UDP_Socket_new(size_t size, char *ip, int port)
{
	UDP_Socket *udp_sock = malloc(size);
    check_mem(udp_sock);

    udp_sock->init = UDP_Socket_init;
    udp_sock->destroy = UDP_Socket_destroy;
    udp_sock->print= UDP_Socket_print;
    udp_sock->connect = UDP_Socket_connect;

    if(udp_sock->init(udp_sock, ip, port) == EXIT_FAILURE) {
        throw("udp socket init failed");
    } else {
        // all done, we made an object of any type
        return udp_sock;
    }

error:
    if(udp_sock) { udp_sock->destroy(udp_sock); };
    return NULL;
}

int UDP_Socket_init(UDP_Socket *this, char *ip, int port)
{
	this->ip = NULL;
    this->port = 0;
    this->localport = 0;
	this->sock_desc = 0;
    this->remote_addr_len = 0;
    this->remote_addr = NULL;
    this->local_addr = NULL;

	this->ip = malloc(strlen(ip) + 1);
    check_mem(this->ip);
    strcpy(this->ip, ip);

	this->port = malloc(sizeof(int));
    check_mem(this->port);
    memcpy(this->port, (void *)&port, sizeof(int));

	return EXIT_SUCCESS;

error:
	return EXIT_FAILURE;
}

void UDP_Socket_destroy(UDP_Socket *this)
{
	if(this){
		close((int) *this->sock_desc);
		free(this->sock_desc);
		if(this->ip) { free(this->ip); };
		if(this->port) { free(this->port); };
		if(this->local_addr) { free((void *)this->local_addr); };
	   	if(this->remote_addr) { free((void *)this->remote_addr); };
		free(this);
	}
}

void UDP_Socket_print(UDP_Socket *this)
{

}

int UDP_Socket_connect(UDP_Socket *this)
{
	/* local address */
    this->local_addr = malloc(sizeof(struct sockaddr_in)); 
   	this->remote_addr = malloc(sizeof(struct sockaddr_in));
   	this->sock_desc = malloc(sizeof(int*));

    /* local address */
    struct sockaddr_in localaddr, remaddr;
    int fd, slen=sizeof(remaddr);
    unsigned int addrlen;

    // create local socket
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        throw(KRED "✗ \ncan't create socket");
        return 0;
    }

    // bind local socket
    memset((void *)&localaddr, 0, sizeof(localaddr));
    localaddr.sin_family = AF_INET;
    localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    localaddr.sin_port = htons(0);
    if (bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) {
        throw("bind failed");
    }

    // name local socket
    addrlen = sizeof(localaddr);
    if (getsockname(fd, (struct sockaddr *)&localaddr, &addrlen) < 0) {
        throw("getsockname failed");
    }

    // get remote location
    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons((int)*this->port);
    if (inet_aton(this->ip, &remaddr.sin_addr)==0) {
        throw("inet_aton() failed\n");
    }

    memcpy((void *)this->local_addr, &localaddr, sizeof(localaddr));
    memcpy(this->sock_desc, &fd, sizeof(int));
    memcpy((void *)this->remote_addr, &remaddr, sizeof(remaddr));
        //if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1){

	return EXIT_SUCCESS;

error:
	return EXIT_FAILURE;
}