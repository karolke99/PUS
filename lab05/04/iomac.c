#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>

void print_MAC_MTU(int sockfd, struct ifreq* ifr)
{
	int retval;
	
	if(retval = ioctl(sockfd, SIOCGIFHWADDR, ifr) == -1) {
		perror("ioctl() error (MAC)");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "MAC: %2x:%2x:%2x:%2x:%2x:%2x\n",
		(unsigned int)ifr->ifr_hwaddr.sa_data[0],
		(unsigned int)ifr->ifr_hwaddr.sa_data[1],
		(unsigned int)ifr->ifr_hwaddr.sa_data[2],
		(unsigned int)ifr->ifr_hwaddr.sa_data[3],
		(unsigned int)ifr->ifr_hwaddr.sa_data[4],
		(unsigned int)ifr->ifr_hwaddr.sa_data[5]);
		
	if(ioctl(sockfd, SIOCGIFMTU, ifr) == -1) {
		perror("ioctl() error (MTU)");
		exit(EXIT_FAILURE);
	}
	
	fprintf(stdout, "MTU: %d\n\n", ifr->ifr_mtu);
}

int main(int argc, char** argv) {

	int sockfd, retval;
	struct ifreq ifr;
	
	if(argc != 4) {
		fprintf(stderr, "Invocation: %s <INTERFACE NAME> <NEW MAC ADDRESS> <NEW MTU>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if ((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() error");
        	exit(EXIT_FAILURE);
	}

    	strcpy(ifr.ifr_name, argv[1]);

    	printf("Before:\n");
    	print_MAC_MTU(sockfd, &ifr);

    	ifr.ifr_hwaddr.sa_family = ARPHRD_ETHER;

    	retval = sscanf(argv[2], "%2x:%2x:%2x:%2x:%2x:%2x",
                 	(unsigned int*)&ifr.ifr_hwaddr.sa_data[0],
                 	(unsigned int*)&ifr.ifr_hwaddr.sa_data[1],
                 	(unsigned int*)&ifr.ifr_hwaddr.sa_data[2],
                 	(unsigned int*)&ifr.ifr_hwaddr.sa_data[3],
                 	(unsigned int*)&ifr.ifr_hwaddr.sa_data[4],
                 	(unsigned int*)&ifr.ifr_hwaddr.sa_data[5]);

    	if (retval != 6) {
        	perror("Invalid address format!");
        	exit(EXIT_FAILURE);
    	}

    	if((retval = ioctl(sockfd, SIOCSIFHWADDR, &ifr)) == -1) {
        	perror("ioctl() overwriting error (MAC)");
        	exit(EXIT_FAILURE);
    	}

    	ifr.ifr_mtu = atoi(argv[3]);

    	if((retval = ioctl(sockfd, SIOCSIFMTU, &ifr)) == -1) {
        	perror("ioctl() overwriting error (MTU)");
        	exit(EXIT_FAILURE);
    	}
    
    	printf("After:\n");
    	print_MAC_MTU(sockfd, &ifr);

    	close(sockfd);
    	exit(EXIT_SUCCESS);
}
