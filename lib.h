#ifndef PEER_TO_PEER_LIB_H_
#define PEER_TO_PEER_LIB_H_
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MY_BUFFER 4096

enum MessageType{ NONE, PEER_MESSAGE, CONNECT_REQUEST };

struct server_info{
    int sockfd;
    in_port_t port;
};

struct client_info{
    uint32_t addr;
    in_port_t port;
};

union connection_info{
    struct server_info server;
    struct client_info client;
};

/*
 * @param char* ip
 * @return unsigned int number representation of the ip
 */
uint32_t get_addr(char* ip);

/*
 * @param char* ip
 * @param int port
 * @param union connection_info*
 * @return int socket_fd
 */
int create_client_socket(unsigned int, int, union connection_info*);

/*
 * @param char* ip
 * @param int port
 * @param union connection_info*
 * @return int socket_fd
 */
int create_server_socket(unsigned int, int, union connection_info*);

/*
 *@param char* message
 *@param union connection_info* peer socket
 *@param in_port_t port number of server
 *@return void
*/
int connect_client(char*, in_port_t, union connection_info*);

/*
 *@param char* message
 *@param union connection_info* peer socket
 *@return void
*/
int send_message(char*, union connection_info*);

/*
 * @param char* message
 * @param union connection_info* peer
 * @param struct sockaddr_in * client address
 * @param int* connected are we connected
 * @return enum MessageType 
 */
enum MessageType parse_message(char*, union connection_info*, struct sockaddr_in*,int*);

#endif
