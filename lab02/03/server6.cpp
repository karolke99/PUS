#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h> 
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]){

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int sockfd;
    int clientfd;
    struct sockaddr_in6 server;
    std::string message = "Laboratorium PUS";
    

    if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) == -1 ) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));
    server.sin6_family = AF_INET6;
    server.sin6_port = htons(port);
    server.sin6_addr = in6addr_any;

    if (bind(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in6)) == -1 ){
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 2) == -1) {
        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    std::cout << "[SERVER] Waiting for client..." << std::endl;

    while (true) {
        struct sockaddr_in6 client;
        socklen_t client_len = sizeof(client);

        if ((clientfd = accept(sockfd, (sockaddr *) &client, &client_len)) == -1 ) {
            perror("accept() error");
            exit(EXIT_FAILURE);
        }

        char* ipClient;
        inet_ntop(AF_INET6, &client.sin6_addr, ipClient, sizeof(client));

        std::cout << "[SERVER] New connection. Client address: ";
        std::cout << ipClient << " : " << ntohs(client.sin6_port) << std::endl;

        if(IN6_IS_ADDR_V4MAPPED(&client.sin6_addr)) {
            std::cout << "[SERVER] Client is IPv4-mapped IPv6." << std::endl;
        } else {
            std::cout << "[SERVER] Client is IPv6." << std::endl;
        }

        if(write(clientfd, message.c_str(), sizeof(message)) <= 0) {
            perror("write() error");
            exit(EXIT_FAILURE);
        }

        close(clientfd);

    }

    exit(EXIT_SUCCESS);

}