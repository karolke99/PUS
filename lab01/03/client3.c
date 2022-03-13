/*
 * Data:                2022-03-06
 * Autor:               Magdalena Żołnierek & Karol Waligóra
 * Kompilacja:          $ gcc client3.c -o client3
 * Uruchamianie:        $ ./client3 <adres IP> <numer portu> 
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main(int argc, char** argv) {

    if(argc != 3) {
        fprintf(
            stderr,
            "Invocation: %s <IPv4 ADDRESS> <PORT> <MESSAGE>\n", argv[0]
        );
        exit(EXIT_FAILURE);
    }

    int sockfd;
    struct sockaddr_in server_addr;

    char* server_address = argv[1];
    int server_port = atoi(argv[2]);

    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("[CLIENT] socket() error.");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_address);
    server_addr.sin_port = htons(server_port);

    if(connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        perror("[CLIENT] connect() error.");
        exit(EXIT_FAILURE);
    }

    char message[256] = "";

    while(1) {

        memset(message,0,sizeof(message));
        
        printf("Type message to send: ");
        fgets(message, sizeof(message), stdin);
        message[strlen(message)-1] = '\0';

        int message_bytes = send(sockfd, message, strlen(message), 0);
        
        if(message_bytes == -1) {
            printf("[CLIENT] Cannot send message\n");
        }else if (message_bytes == 0) {
            sleep(1);
            break;
        } else {
            printf("[CLIENT] Message sent: %s\n", message);
        }

        if(recv(sockfd, message, sizeof(message), 0) == -1) {
            printf("[CLIENT] Cannot receive message\n");
        } else {
            printf("[CLIENT] Received: %s\n", message);
        }

        
    }

    printf("[CLIENT] Closing client...\n");
    close(sockfd);
    exit(EXIT_SUCCESS);

}