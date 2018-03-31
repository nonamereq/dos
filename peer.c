#include <stdio.h>

#include <unistd.h>
#include <sys/select.h>

#include "lib.h"


union connection_info mine, peer;
int connected = 0;

void read_stdin();
void read_socket();

int main(int args, char** argv){
    int socket_fd;
    fd_set read_set, ready_set;

    if(args < 2){
        printf("Usage: peer port\n");
        return 0;
    }

    bzero(&mine, sizeof(mine));
    bzero(&peer, sizeof(peer));
    socket_fd = create_server_socket(get_addr("127.0.0.1"), atoi(argv[1]), &mine);
    if(socket_fd < 0){
        perror("Server creation failed");
        return 1;
    }

    printf("server runing at 127.0.0.1:%d...\n", atoi(argv[1]));

    printf("Enter the server to connect to (ip:port) :\n");

    FD_ZERO(&read_set);
    FD_SET(STDIN_FILENO, &read_set);
    FD_SET(socket_fd, &read_set);

    while(1){
        ready_set = read_set;

        if(select(socket_fd+1, &ready_set, NULL, NULL, NULL) < 0)
            continue;

        if(FD_ISSET(STDIN_FILENO, &ready_set))
            read_stdin();
        else if(FD_ISSET(socket_fd, &ready_set))
            read_socket();
    }
}

void read_stdin(){
    char buf[MY_BUFFER];
    char* needle;

    bzero(buf, MY_BUFFER);

    if(fgets(buf, MY_BUFFER, stdin)){
        if((needle = strchr(buf, '\n')) != NULL)
            *needle = '\0';
        if(connected)
            send_message(buf, &peer);
        else
            connected = connect_client(buf, mine.server.port, &peer);
    }
}

void read_socket(){
    char buf[MY_BUFFER];
    struct sockaddr_in client_addr;
    unsigned int socket_len, nbytes;
    int connfd, old_connected=connected;
    enum MessageType type;

    bzero(buf, MY_BUFFER);


    socket_len = sizeof(client_addr);

    if((connfd = accept(mine.server.sockfd,(struct sockaddr *)&client_addr,&socket_len)) < 0)
        return;

    if(connected && client_addr.sin_addr.s_addr != peer.client.addr)
        return;

    if((nbytes = recv(connfd, buf, MY_BUFFER, 0)) < 0){
        if(nbytes == 0){
            connected = 0;
            printf("Connection ended by peer\n");
            printf("Enter the server to connect to (ip:port) :\n");
            return;
        }
        else{
            perror("receving failed");
            return;
        }
    }

    type = parse_message(buf, &peer, &client_addr, &connected);

    if(!old_connected && connected){
        sprintf(buf, "%c%s", -1, "connected successfully\n");
        printf("Connected to %s. You can now send messages\n", inet_ntoa(client_addr.sin_addr));
    }else if(!connected){
        sprintf(buf, "%c%s", 0, "connection unsucessful\n");
    }
    send(connfd, buf, strlen(buf), 0);
    if(type == PEER_MESSAGE){
        printf("%s: %s\n", inet_ntoa(client_addr.sin_addr), buf);
    }

    close(connfd);
}
