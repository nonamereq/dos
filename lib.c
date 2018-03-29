#include "lib.h"

#define CONNECT_REQUEST_MSG_LEN 20 // 1 byte for the first char 16 bytes for the port number

//forward declaration of atoi. Better than including stdlib.h
int atoi(const char *nptr);

int create_client_socket(char* ip, int port, struct sockets* sock){
    int addr_fd;
    struct sockaddr_in sock_addr;

    if((addr_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    bzero(&sock_addr, sizeof(struct sockaddr_in));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    inet_aton(ip, &sock_addr.sin_addr);

    if(connect(addr_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        return -2;

    sock->client_addr.addr = sock_addr;
    return addr_fd;
}

int create_server_socket(char* ip, int port, struct sockets* sock){
    int addr_fd;
    struct sockaddr_in sock_addr;

    if((addr_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    bzero(&sock_addr, sizeof(struct sockaddr_in));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    inet_aton(ip, &sock_addr.sin_addr);

    if(bind(addr_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        return -2;

    if(listen(addr_fd, 0) < 0)
        return -3;

    sock->server_addr.addr = sock_addr;

    return addr_fd;

}

int connect_client(char* msg, struct sockets* peer){
    char* needle;
    int socket_fd;
    char buf[CONNECT_REQUEST_MSG_LEN];
    if((needle = strchr(msg, ':')) != NULL){
        *needle = '\0';
        ++needle;

        socket_fd = create_client_socket(msg, atoi(needle), peer);
        if(socket_fd > 0){
            sprintf(buf, "%c%d", -1, peer->server_addr.addr.sin_port);

            send(socket_fd, buf, strlen(buf), 0);
            recv(socket_fd, buf, CONNECT_REQUEST_MSG_LEN, 0);

            if(*buf == -1){
                printf("connected successfully to peer\n");
                return 1;
            }
            else{
                printf("peer denied the connection: %s\n", buf+1);
                return 0;
            }
        }
    }
    return 0;
}

int send_message(char* msg, struct sockets* peer){
    int socket_fd = create_client_socket(inet_ntoa(peer->server_addr.addr.sin_addr), peer->server_addr.addr.sin_port, peer);
    if(socket_fd < 0){
        perror("Connection failed with peer");
        return 0;
    }

    send(socket_fd, msg, strlen(msg), 0);
    return 1;
}

static int fill_client_address(char* buffer,struct sockets* peer, struct sockaddr_in *client_addr){
    int connected = 0;
    char* end;
    short port = ntohs(strtol(buffer, &end, 10));
    if(buffer == end)
        return 0;

    peer->client_addr.addr = *client_addr;

    peer->server_addr.addr.sin_family = AF_INET;
    peer->server_addr.addr.sin_port = port;
    peer->server_addr.addr.sin_addr = client_addr->sin_addr;

    return 1;
}

enum MessageType parse_message(char* buffer, struct sockets* peer, struct sockaddr_in *client_addr, int *connected){
    enum MessageType type = NONE;

    if(*buffer == -1){
        if(!(*connected)){
            type = CONNECT_REQUEST;
            *connected = fill_client_address(buffer+1, peer, client_addr);
        }
    }
    else if(*buffer == 0){
        type = PEER_MESSAGE;
        ++buffer;
    }

    return type;
}
