#include "lib.h"

#define CONNECT_REQUEST_MSG_LEN 20 // 1 byte for the first char 16 bytes for the port number

uint32_t get_addr(char* ip){
    struct in_addr addr;
    inet_aton(ip, &addr);
    return addr.s_addr;
}

int create_client_socket(unsigned int addr, int port, union connection_info* info){
    int addr_fd;
    struct sockaddr_in sock_addr;

    if((addr_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    bzero(&sock_addr, sizeof(struct sockaddr_in));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = addr;

    if(connect(addr_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        return -2;

    info->client.addr = addr;

    return addr_fd;
}

int create_server_socket(unsigned int addr, int port, union connection_info* info){
    int addr_fd, yes = 1;
    struct sockaddr_in sock_addr;

    if((addr_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        return -1;

    bzero(&sock_addr, sizeof(struct sockaddr_in));

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = addr;

    setsockopt(addr_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if(bind(addr_fd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) < 0)
        return -2;

    if(listen(addr_fd, 0) < 0)
        return -3;

    info->server.sockfd = addr_fd;
    info->server.port = sock_addr.sin_port;

    return addr_fd;
}

int connect_client(char* address, in_port_t port, union connection_info* info){
    char* needle;
    int socket_fd;
    char buf[CONNECT_REQUEST_MSG_LEN];
    if((needle = strchr(address, ':')) != NULL){
        *needle = '\0';
        ++needle;

        socket_fd = create_client_socket(get_addr(address), atoi(needle), info);
        if(socket_fd > 0){
            sprintf(buf, "%c%d", -1, port);

            send(socket_fd, buf, strlen(buf), 0);
            recv(socket_fd, buf, CONNECT_REQUEST_MSG_LEN, 0);

            if(*buf == -1){
                printf("connected successfully to peer\n");
                info->client.port = atoi(needle);
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

int send_message(char* msg, union connection_info* info){
    union connection_info conn;
    char buf[MY_BUFFER+1];

    int socket_fd = create_client_socket(info->client.addr, info->client.port, &conn);
    if(socket_fd < 0){
        perror("Connection failed with peer");
        return 0;
    }

    sprintf(buf, "%c%s", -2, msg);

    send(socket_fd, buf, strlen(buf), 0);
    return 1;
}

static int fill_client_address(char* buffer, union connection_info* info, struct sockaddr_in *client_addr){
    int connected = 0;
    char* end;
    short port = ntohs(strtol(buffer, &end, 10));
    if(buffer == end)
        return 0;

    info->client.port = port;
    info->client.addr = client_addr->sin_addr.s_addr;


    return 1;
}

enum MessageType parse_message(char* buffer, union connection_info* info, struct sockaddr_in *client_addr, int *connected){
    enum MessageType type = NONE;

    if(*buffer == -1){
        if(!(*connected)){
            type = CONNECT_REQUEST;
            if(client_addr->sin_addr.s_addr == 0)
                client_addr->sin_addr.s_addr = get_addr("127.0.0.1");
            *connected = fill_client_address(buffer+1, info, client_addr);
        }
    }
    else if(*buffer == -2){
        type = PEER_MESSAGE;
        ++buffer;
    }

    return type;
}
