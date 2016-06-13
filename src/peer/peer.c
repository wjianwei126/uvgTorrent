#include "debug/debug.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "utils/sock/socket.h"
#include "peer/peer.h"
#include "peer/packets/peer_handshake_packet.h"
#include "peer/packets/peer_extended_handshake_packet.h"
#include "peer/packets/peer_piece_packet.h"
#include "data_structures/linkedlist/linkedlist.h"
#include "data_structures/hashmap/hashmap.h"
#include "data_structures/hashmap/bucket.h"
#include "utils/bencode/bencode.h"

Peer *Peer_new(size_t size, char *ip, uint16_t port)
{
    Peer *Peer = malloc(size);
    check_mem(Peer);

    Peer->init = Peer_init;
    Peer->destroy = Peer_destroy;
    Peer->print= Peer_print;

    Peer->handshake = Peer_handshake;
    Peer->extended_handshake = Peer_extended_handshake;
    Peer->get_metadata = Peer_get_metadata;

    if(Peer->init(Peer, ip, port) == EXIT_FAILURE) {
        throw("Peer init failed");
    } else {
        // all done, we made an object of any type
        return Peer;
    }

error:
    if(Peer) { Peer->destroy(Peer); };
    return NULL;
}

/**
* int Peer_init(Peer *this, char *path)
*
* Peer    *this; instance to initialize
* char       path; path to file containing magnet link 
* 
* PURPOSE : copy Peer path into Peer struct
* RETURN  : success bool
* NOTES   : magnet link will be taken from stdin
*           once app is working
*/
int Peer_init(Peer *this, char *ip, uint16_t port)
{   
	this->ip = NULL;
    this->port = port;

	this->ip = malloc(strlen(ip) + 1);
    check_mem(this->ip);
    strcpy(this->ip, ip);

    this->socket = NEW(Socket, this->ip, this->port, SOCKET_TYPE_TCP);
    check_mem(this->socket);

    return EXIT_SUCCESS;

error:
	return EXIT_FAILURE;
}

void Peer_destroy(Peer * this)
{   
    if(this) {
        if(this->socket) { this->socket->destroy(this->socket); };
        if(this->ip) { free(this->ip);  };
        free(this);
    }
}

/**
* void Peer_print(Peer *this){
*
* PURPOSE : print a Peer struct
*/
void Peer_print(Peer *this){
    if(this){
        debug("peer :: %s:%u", this->ip, this->port);
    }
}

/**
* void Peer_print(Peer *this){
*
* PURPOSE : print a Peer struct
*/
int Peer_handshake(Peer *this, char * info_hash){
    log_confirm("Attempting Handshake :: %s:%u", this->ip, this->port);

    Peer_Handshake_Packet * handshake = NULL;

    int result = this->socket->connect(this->socket);
    if(result == EXIT_SUCCESS){
    	handshake = NEW(Peer_Handshake_Packet, info_hash, "-UVG012345678912345-");
        int success = 0;
        
        if(handshake->send(handshake, this->socket) == EXIT_SUCCESS){
            int result = EXIT_FAILURE;
            result = handshake->receive(handshake, this->socket);

            if(result == EXIT_SUCCESS){
                success = 1;
                fprintf(stderr, " %s✔%s\n", KGRN, KNRM);
            }
        } else {
            goto error;
        }

        handshake->destroy(handshake);
        if (success == 0) {
            fprintf(stderr, " %s✘%s\n", KRED, KNRM);
        }
    } else {
    	goto error;
    }
	
	return EXIT_SUCCESS;

error:
	if(handshake) { handshake->destroy(handshake); };
    fprintf(stderr, " %s✘%s\n", KRED, KNRM);
	return EXIT_FAILURE;
}

/**
* void Peer_print(Peer *this){
*
* PURPOSE : print a Peer struct
*/
int Peer_extended_handshake(Peer *this){
    log_confirm("Attempting Extended Handshake :: %s:%u", this->ip, this->port);

    Peer_Extended_Handshake_Packet * extended_handshake = NULL;

    extended_handshake = NEW(Peer_Extended_Handshake_Packet);
    int success = 0;
    
    if(extended_handshake->send(extended_handshake, this->socket) == EXIT_SUCCESS){
        int result = EXIT_FAILURE;
        result = extended_handshake->receive(extended_handshake, this->socket);

        if(result == EXIT_SUCCESS){
            if(extended_handshake->response->metadata_size != 0){
                success = 1;
                
                this->ut_metadata = extended_handshake->response->ut_metadata;
                this->metadata_size = extended_handshake->response->metadata_size;
                this->num_pieces = extended_handshake->response->num_pieces;
                this->piece_size = extended_handshake->response->piece_size;

                fprintf(stderr, " %s✔%s\n", KGRN, KNRM);


                debug("ut_metadata :: %i", extended_handshake->response->ut_metadata);
                debug("metadata_size :: %i", extended_handshake->response->metadata_size);
                debug("num_pieces :: %i", extended_handshake->response->num_pieces);
                debug("piece_size :: %i", extended_handshake->response->piece_size);
            }
        } else {

        }
    } else {

    }

    extended_handshake->destroy(extended_handshake);
    if (success == 0) {
        fprintf(stderr, " %s✘%s\n", KRED, KNRM);
        return EXIT_FAILURE;
    } else {
        return EXIT_SUCCESS;
    }
    
error:
    if(extended_handshake) { extended_handshake->destroy(extended_handshake); };
    fprintf(stderr, " %s✘%s\n", KRED, KNRM);
    return EXIT_FAILURE;
}

int Peer_get_metadata(Peer *this, char * out, int metadata_size)
{
    int piece = 0;
    char meta_data[metadata_size * this->num_pieces];
    int metadata_pos = 0;

    int success = 1;
    for (piece = 0; piece < this->num_pieces; piece++){
        Peer_Piece_Packet * piece_packet =  NEW(Peer_Piece_Packet, piece, this->ut_metadata);

        if(piece_packet->send(piece_packet, this->socket) == EXIT_SUCCESS){
            int result = piece_packet->receive(piece_packet, this->socket);
            
            if(result == EXIT_SUCCESS){
                int response_len = piece_packet->response->response_len;

                memcpy(&meta_data[metadata_pos], piece_packet->response->response, response_len);
                metadata_pos += response_len;
            } else {
                success = 0;
                break;
            }

            piece_packet->destroy(piece_packet);
            //return result;
        } else {
            //return EXIT_FAILURE;
        }
    }

    if(success == 1){
        meta_data[metadata_pos+1] = '\0';

        bencode_t * bencoded = malloc(sizeof(bencode_t));
        bencode_init(
            bencoded,
            &meta_data[0],
            metadata_pos);
        
        Hashmap * bencoded_hashmap = bencode_to_hashmap(bencoded);
        free(bencoded);


        /* nested complex data types are a pain to memory manage */
        /* will be refactored so classes can handle proper destroy calls */
        const Linkedlist * files = bencoded_hashmap->get(bencoded_hashmap, "files");
        Bucket * files_bucket = bencoded_hashmap->get_bucket(bencoded_hashmap, "files");
        Linkednode * curr = files->head;
        
        while(curr){
            Hashmap * file = (Hashmap *)curr->get(curr);

            const int * length = file->get(file, "length");

            const Linkedlist * path = file->get(file, "path");
            Linkednode * path_curr = path->head;
            const char * path_str;
            while(path_curr){
                path_str = path_curr->get(path_curr);
                path_curr = path_curr->next;
            }

            log_info_important("%s (%i bytes)", path_str, *length);

            Bucket * path_bucket = file->get_bucket(file, "path");
            path = (const Linkedlist *) path_bucket->value;
            path->destroy((Linkedlist *) path);
            path_bucket->value = NULL;

            file->destroy((Hashmap *) file);
            curr->value = NULL;
            curr = curr->next;
        }

        files = (Linkedlist *) files_bucket->value;
        files->destroy((Linkedlist *)files);
        files_bucket->value = NULL;

        bencoded_hashmap->destroy(bencoded_hashmap);
        bencoded_hashmap = NULL;
    }

    return EXIT_FAILURE;
}