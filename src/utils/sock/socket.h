#ifndef _socket_h
#define _socket_h

#include "macro/macro.h"

typedef enum SOCKET_TYPE
{
    SOCKET_TYPE_TCP,
    SOCKET_TYPE_UDP
} SOCKET_TYPE;

typedef struct Socket Socket;
struct Socket {
    int (*init)(Socket *this, char *ip, int port, SOCKET_TYPE type);
    void (*print)(Socket *this);
    void (*destroy)(Socket *this);
    
    int (*connect)(Socket *this);
    int (*send)(Socket *this, void * message, size_t message_size);
    ssize_t (*receive)(Socket *this, void * buffer, int read_size);

    char * ip;
    int * port;
    int localport;
	int * sock_desc;
    int * remote_addr_len;
    SOCKET_TYPE type;
    const struct sockaddr_in *remote_addr;
    const struct sockaddr_in *local_addr;
};

Socket *Socket_new(size_t size, char *ip, int port, SOCKET_TYPE type);
int Socket_init(Socket *this, char *ip, int port, SOCKET_TYPE type);
void Socket_destroy(Socket *this);
void Socket_print(Socket *this);

int Socket_connect(Socket *this);
int Socket_send(Socket *this, void * message, size_t message_size);
ssize_t Socket_receive(Socket *this, void * buffer, int read_size);

#endif