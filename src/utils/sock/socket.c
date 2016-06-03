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
#include <fcntl.h>
#include <netdb.h>
#include "utils/sock/socket.h"
#include "utils/str/string_utils.h"

Socket *Socket_new(size_t size, char *ip, int port, SOCKET_TYPE type)
{
	Socket *sock = malloc(size);
    check_mem(sock);

    sock->init = Socket_init;
    sock->destroy = Socket_destroy;
    sock->print= Socket_print;

    sock->connect = Socket_connect;
    sock->send = Socket_send;
    sock->receive = Socket_receive;
    if(sock->init(sock, ip, port, type) == EXIT_FAILURE) {
        throw("socket init failed");
    } else {
        // all done, we made an object of any type
        return sock;
    }

error:
    if(sock) { sock->destroy(sock); };
    return NULL;
}

/**
* int Socket_init(Socket *this, char *ip, int port)
*
* Socket    *this; instance to initialize
* char          *ip; remote ip 
* int           *port; remote port 
* 
* PURPOSE : set up a new socket connection
* RETURN  : success bool
*/
int Socket_init(Socket *this, char *ip, int port, SOCKET_TYPE type)
{
	this->ip = NULL;
    this->port = 0;
    this->localport = 0;
	this->sock_desc = 0;
    this->remote_addr_len = 0;
    this->type = type;
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

void Socket_destroy(Socket *this)
{
	if(this){
        if(this->sock_desc){
            close((int) *this->sock_desc);
            free(this->sock_desc);
        }
		if(this->ip) { free(this->ip); };
		if(this->port) { free(this->port); };
		if(this->local_addr) { free((void *)this->local_addr); };
	   	if(this->remote_addr) { free((void *)this->remote_addr); };
		free(this);
	}
}

void Socket_print(Socket *this)
{

}

/**
* int Socket_connect(Socket *this)
*
* Socket    *this; instance to initialize
* 
* PURPOSE : bind, name and connect a new socket
* RETURN  : success bool
*/
int Socket_connect(Socket *this)
{
	/* local address */
    this->local_addr = (this->local_addr == NULL) ? malloc(sizeof(struct sockaddr_in)) : this->local_addr; 
   	this->remote_addr = (this->remote_addr == NULL) ? malloc(sizeof(struct sockaddr_in)) : this->remote_addr;
   	this->sock_desc = (this->sock_desc == 0) ? malloc(sizeof(int*)) : this->sock_desc;

    /* local address */
    struct sockaddr_in localaddr, remaddr;
    int fd;
    unsigned int addrlen;

    // create UDP socket
    if(this->type == SOCKET_TYPE_UDP){
        if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            throw(KRED "✗ \ncan't create socket");
            return 0;
        }   
    } else if(this->type == SOCKET_TYPE_TCP) {
        if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            throw(KRED "✗ \ncan't create socket");
            return 0;
        }   
    }

    struct timeval timeout;

    if(this->type == SOCKET_TYPE_UDP){
        timeout.tv_sec = 1;  /* 30 Secs Timeout */
        timeout.tv_usec = 0;

        setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (struct timeval *)&timeout,sizeof(struct timeval));
        setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&timeout,sizeof(struct timeval));
    }

    if(this->type == SOCKET_TYPE_UDP){
        memset((void *)&localaddr, 0, sizeof(localaddr));
        localaddr.sin_family = AF_INET;
        localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        localaddr.sin_port = htons(0);
    }

    struct hostent *he;
    if ((he=gethostbyname(this->ip)) == NULL) {  /* get the host info */
        herror("gethostbyname");
        exit(1);
    }

    // get remote location
    memset((char *) &remaddr, 0, sizeof(remaddr));
    remaddr.sin_family = AF_INET;
    remaddr.sin_port = htons((int)*this->port);
    remaddr.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(remaddr.sin_zero), 8);     /* zero the rest of the struct */

    /*if (inet_aton(this->ip, &remaddr.sin_addr)==0) {
        throw("inet_aton() failed\n");
    }*/

    if(this->type == SOCKET_TYPE_UDP){
        if (bind(fd, (struct sockaddr *)&localaddr, sizeof(localaddr)) < 0) {
            throw("bind failed");
        }

        // name local socket
        addrlen = sizeof(localaddr);
        if (getsockname(fd, (struct sockaddr *)&localaddr, &addrlen) < 0) {
            throw("getsockname failed");
        }
    } else if(this->type == SOCKET_TYPE_TCP) {
        int res = connect(fd , (struct sockaddr *)&remaddr , sizeof(remaddr));

        if(res < 0) {
            //throw("Error in connection");
            errno=0;
            goto error;
        }
    }

    memcpy((void *)this->local_addr, &localaddr, sizeof(localaddr));
    memcpy(this->sock_desc, &fd, sizeof(int));
    memcpy((void *)this->remote_addr, &remaddr, sizeof(remaddr));

	return EXIT_SUCCESS;

error:
    close(fd);
    free(this->sock_desc);
    this->sock_desc = NULL;
    
	return EXIT_FAILURE;
}

/**
* int Socket_receive(Socket *this, void * output)
*
* Socket    *this; instance to initialize
* void          *output; output pointer
* 
* PURPOSE : copy socket response into output
* RETURN  : success bool
*/
ssize_t Socket_receive(Socket *this, char buffer[2048], int read_size)
{
    int fd = *this->sock_desc;
    struct sockaddr_storage src_addr;

    socklen_t src_addr_len=sizeof(src_addr);

    if(this->type == SOCKET_TYPE_UDP){
        ssize_t count=recvfrom(fd,buffer,read_size,0,(struct sockaddr*)&src_addr,&src_addr_len);
        if (count==read_size) {
            log_warn("datagram too large for buffer: truncated");
        } else {
            if (count == -1) {
                return count;
            }
        }

        return count;
    } else if(this->type == SOCKET_TYPE_TCP) {
        ssize_t count=recv(fd,buffer,read_size,0);
        
        if (count > read_size) {
            log_warn("datagram too large for buffer: truncated");
        } else {
            if (count == -1) {
                return count;
            }
        }

        return count;
    }

    throw("invalid socket type");
    
error:
    return -1;
}

/**
* int Socket_send(Socket *this, void * message, size_t message_size)
*
* Socket    *this; instance to initialize
* void          *message; packet to send (packed struct)
* size_t        message_size; size of the package
* 
* PURPOSE : send a packet (packed struct) to remote server
* RETURN  : success bool
*/
int Socket_send(Socket *this, void * message, size_t message_size)
{
    if(this->type == SOCKET_TYPE_UDP){
        if (sendto(*this->sock_desc, message, message_size, 0, (const struct sockaddr *)this->remote_addr, sizeof(*this->remote_addr)) == -1){
            throw("send failed");
        }
    } else if(this->type == SOCKET_TYPE_TCP) {
        if (send(*this->sock_desc, message, message_size, 0) == -1){
            // throw("send failed");
            errno=0;
            goto error;
        }
    }


    return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}