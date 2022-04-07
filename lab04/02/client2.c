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
    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IP_ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    int retval;

    struct sctp_initmsg initmsg;
    struct sctp_status status;
    struct sockaddr_in server_addr;

    char buffer[BUFF_SIZE];

    if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP)) == -1){
        perror("socket() error");
        exit(EXIT_FAILURE);
    } 

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    retval = inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }


    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = 2;
    initmsg.sinit_max_instreams = 2;
    initmsg.sinit_max_attempts = 5;


    if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg)) != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    if(connect(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    retval = getsockopt(sockfd, IPPROTO_SCTP, SCTP_STATUS, &status, (socklen_t *)sizeof(status));

    printf("================================================\n");
    printf("ID: %d\n", status.sstat_assoc_id);
    printf("State: %d\n", status.sstat_state);
    printf("In streams: %d\n", status.sstat_instrms);
    printf("Out streams: %d\n", status.sstat_outstrms);
    printf("================================================\n");

    while(1) {
        memset(buffer, 0, sizeof(buffer));
        retval = sctp_recvmsg(sockfd, buffer, BUFF_SIZE, (struct sockaddr *)&server_addr, (socklen_t *)sizeof(server_addr), 0, 0);

        if(retval == 0) {
            printf("Closing..");
            break;
        }

        printf("%s", buffer);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);




}