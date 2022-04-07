#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/sctp.h>
#include <netinet/in.h>
#include <time.h>

#define BUFF_SIZE 256

int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int listenfd, connfd;
    int retval;

    struct sockaddr_in server_addr;
    struct sctp_initmsg initmsg;


    char buffer[BUFF_SIZE];

    time_t rawtime;
    struct tm* timeinfo;

    if((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[1]));
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0 ,sizeof(initmsg));
    initmsg.sinit_num_ostreams = 3;
    initmsg.sinit_max_instreams = 4;
    initmsg.sinit_max_attempts = 5;

    if (setsockopt(
                    listenfd, 
                    IPPROTO_SCTP,
                    SCTP_INITMSG,
                    &initmsg,
                    sizeof(initmsg)
                    ) != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    if (listen(listenfd, 5) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening for incoming connection...\n");

    while(1) {
        connfd = accept(listenfd, NULL, 0);
        if (connfd == -1) {
            perror("accept()");
            exit(EXIT_FAILURE);
        }

        sleep(1);

        printf("Sending current date..\n");
        
        memset(&buffer, 0, sizeof(buffer));
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);

        retval = sctp_sendmsg(connfd, buffer, (size_t)strlen(buffer), NULL, 0,0,0,0,0,0);
        if(retval == -1) {
            perror("sctp_sendmsg() date error");
            exit(EXIT_FAILURE);
        }

        printf("Sending current time..\n");
        
        memset(&buffer, 0, sizeof(buffer));
        strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);

        retval = sctp_sendmsg(connfd, buffer, (size_t)strlen(buffer), NULL, 0,0,0,1,0,0);
        if(retval == -1) {
            perror("sctp_sendmsg() time error");
            exit(EXIT_FAILURE);
        }
    }

    close(listenfd);
    exit(EXIT_SUCCESS);

}