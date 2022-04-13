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
        fprintf(stderr, "Invocation: %s <PORT> <MODE: 0/1>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int listenfd;
    int retval;

    struct sockaddr_in serveraddr;
    struct sockaddr clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);

    struct sctp_initmsg initmsg;
    struct sctp_sndrcvinfo sndrcvinfo;
    struct sctp_event_subscribe events;

    char buffer[BUFF_SIZE];

    int mode = atoi(argv[2]);

    int current_stream = 0;
    int send_stream = 0;

    listenfd = socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);
    if(listenfd == -1) {
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(atoi(argv[1]));
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(listenfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind() error");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = NUM_STREAMS;
    initmsg.sinit_max_instreams = NUM_STREAMS;
    initmsg.sinit_max_attempts = 5;

    retval = setsockopt(listenfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
    if(retval != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    memset(&events, 0, sizeof(events));
    events.sctp_data_io_event = 1;
    retval = setsockopt(listenfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));
    if(retval != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    if(listen(listenfd, 5) == -1) {
        perror("listen() error");
        exit(EXIT_FAILURE);
    }

    while(1) {

        memset(buffer, 0, sizeof(buffer));
        if(mode) {
            current_stream = send_stream;
            if((send_stream + 1) == NUM_STREAMS) {
                send_stream = -1;
            }
            ++send_stream;
        }

        retval = sctp_recvmsg(listenfd, buffer, BUFF_SIZE, &clientaddr, &clientaddr_len, &sndrcvinfo, NULL);
        if(retval == -1) {
            perror("sctp_recvmsg() error");
            exit(EXIT_FAILURE);
        }
        printf("===========================================\n");
        printf("Stream number: %d\n", current_stream);
        printf("ID: %d\n", sndrcvinfo.sinfo_assoc_id);
        printf("SSN: %d\n", sndrcvinfo.sinfo_ssn);
        printf("===========================================\n");

        retval = sctp_sendmsg(listenfd, (void*)buffer, BUFF_SIZE, &clientaddr, clientaddr_len, 0, 0, current_stream, 0, 0);
        if(retval == -1){
            perror("sctp_sendmsg() error");
            exit(EXIT_FAILURE);
        }
    }

    close(listenfd);
    exit(EXIT_SUCCESS);

}