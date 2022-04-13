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
#define NUM_STREAMS 3

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd;
    int retval;
    int bytes;
    char *retptr;

    struct addrinfo hints, *result;

    struct sctp_initmsg initmsg;
    //struct sctp_status status;
    struct sctp_sndrcvinfo sndrcvinfo;
    struct sctp_event_subscribe events;

    char buffer[BUFF_SIZE];
    int stream = 0;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = 0;
    hints.ai_protocol = 0;

    retval = getaddrinfo(argv[1], argv[2], &hints, &result);
    if (retval != 0) {
        perror("getaddrinfo() error");
        exit(EXIT_FAILURE);
    }
    if (result == NULL) {
        fprintf(stderr, "Could not connect!\n");
        exit(EXIT_FAILURE);
    }

    sockfd = socket(result->ai_family, result->ai_socktype, IPPROTO_SCTP);
    if(sockfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = NUM_STREAMS;
    initmsg.sinit_max_instreams = NUM_STREAMS;
    initmsg.sinit_max_attempts = 5;

    memset(&events, 0, sizeof(events));
    events.sctp_data_io_event = 1;

    retval = setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
    if(retval != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    retval = setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));
    if(retval != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result);

    while(1) {
        retptr = fgets(buffer, BUFF_SIZE, stdin);
        if ((retptr == NULL) || (strcmp(buffer, "\n") == 0)) {
            break;
        }

        bytes = sctp_sendmsg(sockfd, (void*)buffer, BUFF_SIZE, result->ai_addr, result->ai_addrlen, 0, 0, stream, 10, 0);
        if(retval == -1) {
            perror("sctp_sendmsg() error");
            exit(EXIT_FAILURE);
        }

        memset(&buffer, 0, BUFF_SIZE);
        bytes = sctp_recvmsg(sockfd, buffer, BUFF_SIZE, result->ai_addr, &(result->ai_addrlen), &sndrcvinfo, 0);

        if(bytes == -1) {
            perror("sctp_recvmsg() error");
            exit(EXIT_FAILURE);
        }

        printf("===========================================\n");
        printf("Stream number: %d\n", sndrcvinfo.sinfo_stream);
        printf("ID: %d\n", sndrcvinfo.sinfo_assoc_id);
        printf("SSN: %d\n", sndrcvinfo.sinfo_ssn);
        buffer[bytes] = 0;
        printf("Message from server: %s\n", buffer);
        printf("===========================================\n");

        stream = sndrcvinfo.sinfo_stream;

        memset(&buffer, 0, BUFF_SIZE);

    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}
