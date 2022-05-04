#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <asm/types.h>
#include <arpa/inet.h>
#include <net/if.h>

#define MYPROTO NETLINK_ROUTE 
#define MYMGRP RTMGRP_IPV4_ROUTE

int open_netlink() {
	int sock = socket(AF_NETLINK, SOCK_RAW, MYPROTO);
        struct sockaddr_nl addr;

        memset((void *) &addr, 0, sizeof(addr));

        if(sock < 0) return sock;
            
        addr.nl_family = AF_NETLINK;
        addr.nl_pid = getpid();
        addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR;
        
        if(bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        	perror("bind() error");
            	exit(EXIT_FAILURE);
        }
        return sock;
}

static int msg_handler(struct sockaddr_nl *netl, struct nlmsghdr *msg) {
        struct ifinfomsg *ifi = NLMSG_DATA(msg); 
        struct ifaddrmsg *ifa = NLMSG_DATA(msg); 
        char ifname[1024];
        int len;
        
        switch(msg->nlmsg_type) {
            case RTM_NEWADDR:
                if_indextoname(ifa->ifa_index,ifname);
                printf("%s: NEW IP ADDRESS (RTM_NEWADDR)\n", ifname);
                break;
            
            case RTM_DELADDR:
                if_indextoname(ifa->ifa_index,ifname);
                printf("%s: DELETED IP ADDRESS (RTM_DELADDR)\n", ifname);
                break;
            
            case RTM_NEWLINK:
                if_indextoname(ifi->ifi_index, ifname);
                printf("%s: NEW LINK CREATED OR EDITED (RTM_NEWLINK)\n", ifname);
                printf("%s: Link %s\n", ifname, (ifi->ifi_flags & IFF_UP)?"Up":"Down");
                break;
            
            case RTM_DELLINK:
                if_indextoname(ifi->ifi_index,ifname);
                printf("%s: DELETED LINK (RTM_DELLINK)\n", ifname);
                break;
            
            default:
                printf("%s: Unknown netlink message type: %d\n", ifname, msg->nlmsg_type);
                break;
        }
        return 0;
}

int event_reader(int sockint, int (*msg_handler)(struct sockaddr_nl *, struct nlmsghdr *)) {
        int ret; 
        char buf[4096];
        
        struct iovec iov = { buf, sizeof buf };
        struct sockaddr_nl snl;
        struct msghdr msg = { (void*) &snl, sizeof snl, &iov, 1, NULL, 0, 0 };
        struct nlmsghdr *hdr;

        int status = recvmsg(sockint, &msg, 0);

        if(status < 0) {
            	if (errno == EWOULDBLOCK || errno == EAGAIN)
                	return 0;

            	printf("event_reader: Error recvmsg: %d\n", status);
            	perror("event_reader: Error: ");
            	exit(EXIT_FAILURE);
        }

        if(status == 0) printf("event_reader: EOF\n");

        for(hdr = (struct nlmsghdr *) buf; NLMSG_OK (hdr, (unsigned int) status); hdr = NLMSG_NEXT(hdr, status)) {       
            	if (hdr->nlmsg_type == NLMSG_DONE)
                	return 0;

            	if (hdr->nlmsg_type == NLMSG_ERROR) {
                	perror("event_reader: Error message\n");
                	exit(EXIT_FAILURE);
            	}
            	
            	if(msg_handler) {
                	ret = (*msg_handler)(&snl, hdr);
              		if(ret < 0) {
                    		printf("event_reader: msg_handler error %d\n", ret);
                    		exit(EXIT_FAILURE);
              		}
            	}
            	else {
                	perror("event_reader: msg_handler NULL error\n");
                	exit(EXIT_FAILURE);
            	}
        }
        return ret;
}
    
int main(int argc, char *argv[]) {
        int netl_sock = open_netlink();
        printf("Looking for events...\n");
        if (netl_sock < 0) {
            perror("Launching error");
            exit(EXIT_FAILURE);
        }
        while (1)
            event_reader(netl_sock, msg_handler);
        exit(EXIT_SUCCESS);
}
