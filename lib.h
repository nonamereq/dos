#ifndef PEER_TO_PEER_LIB_H_
#define PEER_TO_PEER_LIB_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

enum MessageType{ NONE, PEER_MESSAGE, CONNECT_REQUEST };

struct socket_disc {
    struct sockaddr_in addr;
    int sock_fd;
};

struct sockets{
    struct socket_disc server_addr;
    struct socket_disc client_addr;
};

/*
 * @param char* ip
 * @param int port
 * @param struct socket
 * @return int socket_fd
 */
int create_client_socket(char*, int, struct sockets*);

/*
 * @param char* ip
 * @param int port
 * @param struct socket
 * @return int socket_fd
 */
int create_server_socket(char*, int, struct sockets*);

/*
 *@param char* message
 *@param struct sockets* peer socket
 *@return void
*/
int connect_client(char*, struct sockets*);

/*
 *@param char* message
 *@param struct sockets* peer socket
 *@return void
*/
int send_message(char*, struct sockets*);

/*
 * @param char* message
 * @param struct sockets* peer
 * @param struct sockaddr_in * client address
 * @param int* connected are we connected
 * @return enum MessageType 
 */
enum MessageType parse_message(char*, struct sockets*, struct sockaddr_in*, int*);

#endif
