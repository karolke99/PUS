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
    int msg_flags;

    struct sctp_initmsg initmsg;
    struct sctp_status status;
    struct sctp_event_subscribe events;
    struct sctp_sndrcvinfo sndrcvinfo;
    //struct sockaddr_in server_addr;
    struct addrinfo hints, *result;

    char buffer[BUFF_SIZE];

    memset(&hints, 0, sizeof(struct addrinfo));
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

    if((sockfd = socket(result->ai_family, result->ai_socktype, IPPROTO_SCTP)) == -1){
        perror("socket() error");
        exit(EXIT_FAILURE);
    }

    memset(&initmsg, 0, sizeof(initmsg));
    initmsg.sinit_num_ostreams = 3;
    initmsg.sinit_max_instreams = 4;
    initmsg.sinit_max_attempts = 5;

    if(setsockopt(sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg)) != 0) {
        perror("setsockopt() error");
        exit(EXIT_FAILURE);
    }

    if(connect(sockfd,result->ai_addr, result->ai_addrlen) == -1) {
        perror("connect() error");
        exit(EXIT_FAILURE);
    }

    memset(&events, 0, sizeof(events));
    events.sctp_data_io_event = 1;

    setsockopt(sockfd, IPPROTO_SCTP, SCTP_EVENTS, &events, sizeof(events));

    getsockopt(sockfd, IPPROTO_SCTP, SCTP_STATUS, (void *)&status, (socklen_t *)sizeof(status));
    
    sleep(1);

    printf("================================================\n");
    printf("ID: %d\n", status.sstat_assoc_id);
    printf("State: %d\n", status.sstat_state);
    printf("In streams: %d\n", status.sstat_instrms);
    printf("Out streams: %d\n", status.sstat_outstrms);
    printf("================================================\n");

    for (;;) 
    {
        memset(buffer, 0, sizeof(buffer));
        retval = sctp_recvmsg(sockfd, (void *) buffer, BUFF_SIZE,(struct sockaddr *) NULL, 0, &sndrcvinfo, &msg_flags);

        if (retval > 0)
	    {
            buffer[retval] = '\0';
            printf ("(Nr strumienia: %d) %s\n", sndrcvinfo.sinfo_stream, buffer);
        } else if(retval == 0) {
            close(sockfd);
            exit(EXIT_SUCCESS);
        }
    }

}