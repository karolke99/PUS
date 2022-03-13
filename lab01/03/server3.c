/*
 * Data:                2022-03-06
 * Autor:               Magdalena Żołnierek & Karol Waligóra
 * Kompilacja:          $ gcc -o server3 server3.c libpalindrome.c
 * Uruchamianie:        $ ./server3 <adres IP> <numer portu> 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>

#include "libpalindrome.h"

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;         //Socket descriptor
    int retval;         //Returned value

    //Socket address structures (client and server)
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    
    //Structures' size in bytes
    socklen_t client_addr_len;
    socklen_t server_addr_len;


    char buff[256] = "";        //recvfrom() and sendto() buffer
    char addr_buff[256];        //client IP address buffer

    int server_port = atoi(argv[1]);

    // UDP protocol socket creation:
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("[SERVER] socket() error");
        exit(EXIT_FAILURE);
    } else {
        printf("[SERVER] Socket has been created\n");
    }

    //clear server's address structure:
    memset(&server_addr, 0, sizeof(server_addr));

    //communication domain (protocol family)
    server_addr.sin_family = AF_INET;
    //wildcard address
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //port number
    server_addr.sin_port = htons(server_port);

    //size of server's address structure
    server_addr_len = sizeof(server_addr);

    //Binding IP address and port number with socket
    if (bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len) == -1 ) {
        perror("[SERVER] bind() error");
        exit(EXIT_FAILURE);
    } else {
        printf("[SERVER] Socket has got its name\n");
    }

    printf("[SERVER] Waiting for clients...\n");
    client_addr_len = sizeof(client_addr);

    while(1) {
        
        //reset message buffer
        memset(buff,0,sizeof(buff));

        //waiting for client's data
        retval = recvfrom(sockfd, buff, sizeof(buff), 0, (struct sockaddr*)&client_addr, &client_addr_len);

        if(retval == -1) {
            perror("[SERVER] recvfrom() error. Cannot get message from client.");
            exit(EXIT_FAILURE);
        } else {
            printf("[SERVER] New client connection. Client: %s:%d\n",
                    inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
                    ntohs(client_addr.sin_port)
                    );
        }

        printf("[SERVER] Incoming message: %s \n", buff);
        //checking message size - if empty, close socket and stop server
        if(strlen(buff) == 0) {
            printf("[SERVER] Closing server...\n");
            close(sockfd);
            exit(EXIT_SUCCESS);
        }

        char* response;


        //check if incoming message is palindrome, send response
        if(is_palindrome(buff, strlen(buff)) == -1) {
            response = "SERVER: Cannot verify data. Wrong format.\0";
        } else if (is_palindrome(buff, strlen(buff)) == 1) {
            response = "SERVER: Is palindrome.\0";
        } else {
            response = "SERVER: Is not palindrome.\0";
        }

        sendto(sockfd, response, strlen(response), 0 , (struct sockaddr *) &client_addr, client_addr_len);

        response = "\0";
    }


}