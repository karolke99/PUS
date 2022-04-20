#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h> /* inet_pton() */
#include <net/if_arp.h>
#include <netinet/in.h> /* struct sockaddr_in */
#include <sys/ioctl.h>
#include <net/if.h>

int main(int argc, char** argv) {

    int sockfd, retval;
    int mode;

    struct ifreq ifr;
    struct ifreq helper;

    struct sockaddr_in *temp;

    memset(&ifr, 0, sizeof(struct ifreq));
    strcpy(ifr.ifr_name, argv[1]);

    
    memset(&helper, 0, sizeof(struct ifreq));
    strcpy(helper.ifr_name, argv[1]);

    if(!(argc == 3 || argc == 5)) {
        fprintf(stderr, "Invocation: %s <INTERFACE> add <IPv4> <subnet_mask>\n", argv[0]);
        fprintf(stderr, "Invocation: %s <INTERFACE> down\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(strcmp(argv[2], "add") == 0) {
        mode = 1;
    } else if(strcmp(argv[2], "down") == 0) {
        mode = 0;
    } else {
        fprintf(stderr, "Invocation: %s <INTERFACE> add <IPv4> <subnet_mask>\n", argv[0]);
        fprintf(stderr, "Invocation: %s <INTERFACE> down\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if(mode) {
        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("socket() error");
            exit(EXIT_FAILURE);
        }

        printf("Add....\n");
        if(ioctl(sockfd, SIOCGIFFLAGS, &helper) == -1){
            perror("ioctl() error");
        }
        
        if(!(helper.ifr_flags & IFF_UP)) {
            fprintf(stderr, "Interface not up.");
            exit(EXIT_FAILURE);
        }
        
        ifr.ifr_addr.sa_family = AF_INET;
        temp = (struct sockaddr_in*)&ifr.ifr_addr;

        ifr.ifr_netmask.sa_family = AF_INET;

        if(inet_pton(AF_INET, argv[3], &temp->sin_addr) != 1) {
            perror("inet_pton() error here");
            exit(EXIT_FAILURE);
        }
        
        if(ioctl(sockfd, SIOCSIFADDR, &ifr) == -1) {
            perror("ioctl() error");
            exit(EXIT_FAILURE);
        }
        
        if(inet_pton(AF_INET, argv[4], &temp->sin_addr) != 1) {
            printf("%s", ifr.ifr_netmask.sa_data);
            perror("inet_pton() error here 2");
            exit(EXIT_FAILURE);
        }

        if(ioctl(sockfd, SIOCSIFNETMASK, &ifr) == -1) {
            perror("ioctl() error itshere");
            exit(EXIT_FAILURE);
        }
    } else {
        if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
            perror("socket() error");
            exit(EXIT_FAILURE);
        }

        ifr.ifr_flags &= ~IFF_UP;
        if(ioctl(sockfd, SIOCSIFFLAGS, &ifr) == -1) {
            perror("ioctl() address remove error");
            exit(EXIT_FAILURE);
        }

    }

    close(sockfd);

    exit(EXIT_SUCCESS);
}