#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);

    int sockfd, connfd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t server_addr_len = sizeof(server_addr);
    socklen_t client_addr_len = sizeof(client_addr);
    char addr_buff[256];

    std::string message = "Laboratorium PUS";

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socker() error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if((bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len)) == -1) {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    if(listen(sockfd, 2) == -1) {
        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    std::cout << "[SERVER] Waiting for client..." << std::endl;

    while (true)
    {
        if((connfd = accept(sockfd, (sockaddr*) &client_addr, &client_addr_len)) < 0) {
            perror("accept() error");
            exit(EXIT_FAILURE);
        }

        std::cout << "[SERVER] New connection. Client address: ";
        std::cout << inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff));
        std::cout << " : " << ntohs(client_addr.sin_port) << std::endl;

        if(write(connfd, message.c_str(), sizeof(message)) <= 0) {
            perror("write() error");
            exit(EXIT_FAILURE);
        }
    }
    
    
}