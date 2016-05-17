/***************************************************************************/
/*                                                                         */    
/* Client program which gets as parameter the server name or               */    
/*     address and tries to send the data into non-blocking server.        */    
/*                                                                         */    
/* The message is sent after 5 seconds of wait                             */    
/*                                                                         */    
/*                                                                         */    
/* based on Beej's program - look in the simple TCp client for further doc.*/    
/*                                                                         */    
/*                                                                         */    
/***************************************************************************/
    #include <stdio.h> 
    #include <stdlib.h> 
    #include <errno.h> 
    #include <string.h> 
    #include <netdb.h> 
    #include <sys/types.h> 
    #include <netinet/in.h> 
    #include <sys/socket.h> 
    #include <unistd.h>

    #define MAXDATASIZE 100 /* max number of bytes we can get at once */

    int main(int argc, char *argv[])
    {
        int port = strtol(argv[2], NULL, 10);
        int sockfd, numbytes;  
        char buf[MAXDATASIZE];
        struct hostent *he;
        struct sockaddr_in their_addr; /* connector's address information */

        if (argc != 3) {
            fprintf(stderr,"usage: client hostname\n");
            exit(1);
        }

        if ((he=gethostbyname(argv[1])) == NULL) {  /* get the host info */
            herror("gethostbyname");
            exit(1);
        }

        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket");
            exit(1);
        }

        their_addr.sin_family = AF_INET;      /* host byte order */
        their_addr.sin_port = htons(port);    /* short, network byte order */
        their_addr.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

        if (connect(sockfd, (struct sockaddr *)&their_addr, \
                                              sizeof(struct sockaddr)) == -1) {
            perror("connect");
            exit(1);
        }
        while (1) {
                /* prepare handshake message */
                char bytes[68];

                int8_t pstrlen = 19;
                char * pstr = "BitTorrent protocol";
                char * info_hash = "9609f0336566953f3bf342241b25e2437f65b2c8";
                char * peer_id = "-UVG012345678912345-";

                int8_t info_hash_bytes[20];
                /* hex string to int8_t array */
                int count = 0;
                for(count = 0; count < sizeof(info_hash_bytes); count++) {
                    sscanf(info_hash, "%2hhx", &info_hash_bytes[count]);
                    info_hash += 2 * sizeof(char);
                }

                /* store packet data in byte array for sending */
                size_t pos = 0;
                memcpy(&bytes[pos], &pstrlen, sizeof(char));
                pos += sizeof(char);

                memcpy(&bytes[pos], &pstr, strlen(pstr) * sizeof(int8_t));
                pos += strlen(pstr) * sizeof(int8_t);

                bytes[pos] = 0x0;
                bytes[pos+1] = 0x0;
                bytes[pos+2] = 0x0;
                bytes[pos+3] = 0x0;
                bytes[pos+4] = 0x0;
                bytes[pos+5] = 0x0;
                bytes[pos+6] = 0x0;
                bytes[pos+7] = 0x0;
                pos += 8;

                memcpy(&bytes[pos], &info_hash_bytes, 20 * sizeof(int8_t));
                pos += 20 * sizeof(int8_t);

                memcpy(&bytes[pos], &peer_id, strlen(peer_id) * sizeof(int8_t));
                pos += strlen(peer_id) * sizeof(int8_t);


                if (send(sockfd, &bytes, 68, 0) == -1){
                      perror("send");
                      exit (1);
                }
                printf("After the send function \n");

                if ((numbytes=recv(sockfd, buf, MAXDATASIZE, 0)) == -1) {
                            perror("recv");
                            exit(1);
                }        

                //buf[numbytes] = '\0';

                int post = 0;
                pstrlen = 0;

                memcpy(&pstrlen, &buf[pos], sizeof(int8_t));
                pstrlen = ntohs(pstrlen);
                pos += sizeof(int8_t);
                printf("PSTRLEN :: %i", pstrlen);
                printf()

                printf("Received in pid=%d, text=: %s \n",getpid(), buf);
                sleep(1);

        }

        close(sockfd);

        return 0;
    }


