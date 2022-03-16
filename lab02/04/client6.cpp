/*
    Run: $ ./client6 ::ffff:127.0.0.1 <PORT> lo
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>
#include <net/if.h>

int main(int argc, char* argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Invocation: %s <IPv6 ADDRESS> <PORT> <INTERFACE>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in6 remote_addr;
    socklen_t addr_len;
    addr_len = sizeof(remote_addr);
    char buff[256];

    if((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    std::string ip = argv[1];
    std::string interface = argv[3];
    int port = atoi(argv[2]);

    memset(&remote_addr, 0, sizeof(remote_addr));
    remote_addr.sin6_family = AF_INET6;
    remote_addr.sin6_port = htons(port);
    
    if(if_nametoindex(interface.c_str()) == 0) {
        perror("if_nametoindex() error");
        exit(EXIT_FAILURE);
    }

    remote_addr.sin6_scope_id = if_nametoindex(interface.c_str());
    inet_pton(AF_INET6, ip.c_str(), &remote_addr.sin6_addr);

    if(connect(sockfd, (struct sockaddr*) &remote_addr, sizeof(remote_addr)) == -1){
        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    if(recv(sockfd, buff, sizeof(buff), 0) == -1){
        perror("recv() error");
        exit(EXIT_FAILURE);
    }

    std::cout << "[CLIENT] Recived from server: " << buff << std::endl;

    std::cout << "[CLIENT] Closing..." << std::endl;


    close(sockfd);
    exit(EXIT_SUCCESS);
}