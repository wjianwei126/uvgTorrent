#ifndef _upd_socket_h
#define _upd_socket_h

#include "macro/macro.h"

typedef struct UDP_Socket UDP_Socket;
struct UDP_Socket {
    int (*init)(UDP_Socket *this, char *ip, int port);
    void (*print)(UDP_Socket *this);
    void (*destroy)(UDP_Socket *this);
    
    int (*connect)(UDP_Socket *this);
    int (*send)(UDP_Socket *this, void * message, size_t message_size);
    ssize_t (*receive)(UDP_Socket *this, signed char buffer[2048]);

    char * ip;
    int * port;
    int localport;
	int * sock_desc;
    int * remote_addr_len;
    const struct sockaddr_in *remote_addr;
    const struct sockaddr_in *local_addr;
};

UDP_Socket *UDP_Socket_new(size_t size, char *ip, int port);
int UDP_Socket_init(UDP_Socket *this, char *ip, int port);
void UDP_Socket_destroy(UDP_Socket *this);
void UDP_Socket_print(UDP_Socket *this);

int UDP_Socket_connect(UDP_Socket *this);
int UDP_Socket_send(UDP_Socket *this, void * message, size_t message_size);
ssize_t UDP_Socket_receive(UDP_Socket *this, signed char buffer[2048]);

#endif