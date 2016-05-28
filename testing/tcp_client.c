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
        int pos = 0;
        char message[69] = "00000000000000000000000000000000000000000000000000000000000000000000\0";
        char * protocol_string = "BitTorrent protocol";
        message[0] = (char)19;
        pos += 1;

        strcpy(&message[pos], protocol_string);
        pos += strlen(protocol_string);
        
        int i;
        for (i = 0; i < 8; i++){
            if (i == 5){
                message[pos] = '\x10';
            } else {
                message[pos] = (char)0;
            }
            pos += 1;
        }

        char * info_hash = "9609f0336566953f3bf342241b25e2437f65b2c8";
        char * peer_id = "-UVG012345678912345-";
        int8_t info_hash_bytes[20];
        /* hex string to int8_t array */
        int count = 28;
        for(count = 28; count < 28 + sizeof(info_hash_bytes); count++) {
            sscanf(info_hash, "%2hhx", &message[count]);
            info_hash += 2 * sizeof(char);
            pos += 1;
        }

        strcpy(&message[pos], peer_id);
        pos += strlen(peer_id);

        // INFO HASH // 

        printf("%s", message);
        pos = 0;
int port = strtol(argv[2], NULL, 10);
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



their_addr.sin_family = AF_INET;      /* host byte order */
their_addr.sin_port = htons(port);    /* short, network byte order */
their_addr.sin_addr = *((struct in_addr *)he->h_addr);
bzero(&(their_addr.sin_zero), 8);     /* zero the rest of the struct */

while (1) {
    int sockfd, numbytes;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }  

    if (connect(sockfd, (struct sockaddr *)&their_addr, \
                                          sizeof(struct sockaddr)) == -1) {
        perror("connect");
        exit(1);
    }
        /* prepare handshake message */
        //char bytes[68];
//
        int8_t pstrlen = htons(19);
        //char * pstr = "BitTorrent protocol";
        //char * info_hash = "9609f0336566953f3bf342241b25e2437f65b2c8";
        //char * peer_id = "-UVG012345678912345-";
//
        //int8_t info_hash_bytes[20];
        ///* hex string to int8_t array */
        //int count = 0;
        //for(count = 0; count < sizeof(info_hash_bytes); count++) {
        //    sscanf(info_hash, "%2hhx", &info_hash_bytes[count]);
        //    info_hash += 2 * sizeof(char);
        //}

        if (send(sockfd, message, 68, 0) == -1){
              //perror("send");
              //exit (1);
        }
        printf("After the send function \n");
        numbytes = -1;
        while (numbytes < 0){
            if ((numbytes=recv(sockfd, buf, 68, 0)) == -1) {
                        //perror("recv");
                        //exit(1);
            } else {
                break;
            }    
        }

        //buf[numbytes] = '\0';

        int pos = 0;
        pstrlen = 0;

        memcpy(&pstrlen, &buf[pos], sizeof(int8_t));
        //pstrlen = ntohs(pstrlen);
        printf("PSTRLEN :: %i", pstrlen);
        printf("numbytes :: %i", numbytes);

        printf("Received in pid=%d, text=: %s \n",getpid(), buf);

        char meta_message[30];
        char * bencoded_message = "d1:md11:ut_metadatai1eee";
    
        uint32_t length = htonl(26);
        uint8_t bt_msg_id = 20;
        uint8_t handshake_id = 0; 

        pos = 0;
        memcpy(&meta_message[pos], &length, sizeof(uint32_t));
        pos += sizeof(uint32_t);

        memcpy(&meta_message[pos], &bt_msg_id, sizeof(uint8_t));
        pos += sizeof(uint8_t);

        memcpy(&meta_message[pos], &handshake_id, sizeof(uint8_t));
        pos += sizeof(uint8_t);

        memcpy(&meta_message[pos], bencoded_message, strlen(bencoded_message));
        pos += strlen(bencoded_message);

        /*meta_message[0] = '\x00';
        meta_message[1] = '\x00';
        meta_message[2] = '\x00';
        meta_message[3] = '\x1a';
        meta_message[4] = '\x14';
        meta_message[5] = '\x00';
        meta_message[6] = 'd';
        meta_message[7] = '1';
        meta_message[8] = ':';
        meta_message[9] = 'm';
        meta_message[10] = 'd';
        meta_message[11] = '1';
        meta_message[12] = '1';
        meta_message[13] = ':';
        meta_message[14] = 'u';
        meta_message[15] = 't';
        meta_message[16] = '_';
        meta_message[17] = 'm';
        meta_message[18] = 'e';
        meta_message[19] = 't';
        meta_message[20] = 'a';
        meta_message[21] = 'd';
        meta_message[22] = 'a';
        meta_message[23] = 't';
        meta_message[24] = 'a';
        meta_message[25] = 'i';
        meta_message[26] = '1';
        meta_message[27] = 'e';
        meta_message[28] = 'e';
        meta_message[29] = 'e';*/

        if (send(sockfd, &meta_message, 30, 0) == -1){
              //perror("send");
              //exit (1);
        }
        printf("After the metadata function \n");
        char metadata[101];
        numbytes = -1;
        while (numbytes < 0){
            if ((numbytes=recv(sockfd, metadata, 100, 0)) == -1) {
                        //perror("recv");
                        //exit(1);
            } else {
                break;
            }    
        }

        printf("numbytes :: %i \n", numbytes);
        printf("got message: %s \n", &metadata[10]);

        close(sockfd);
        sleep(1);
}

return 0;
    }


