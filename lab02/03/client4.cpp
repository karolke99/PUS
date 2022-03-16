//
// Created by Karol on 16.03.2022.
//

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
    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    struct sockaddr_in server;
    socklen_t server_addrlen;
    server_addrlen = sizeof(server);

    int clientfd;
    char buff[256];

    if ((clientfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, argv[1], &server.sin_addr);

    if (connect(clientfd, (sockaddr *) &server, server_addrlen) == -1){
        perror("connect() error");
        exit(EXIT_FAILURE);
    }


    if(recv(clientfd, buff, sizeof(buff), 0) == -1){
        perror("recv() error");
        exit(EXIT_FAILURE);
    }

    std::cout << "[CLIENT] Recived from server: " << buff << std::endl;

    std::cout << "[CLIENT] Closing..." << std::endl;
    sleep(1);



    close(clientfd);

    exit(EXIT_SUCCESS);
}