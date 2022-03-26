#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[2]);
    
    addrinfo whichIP;
    addrinfo * serverList;
    
    whichIP.ai_family = AF_UNSPEC;
    whichIP.ai_socktype = SOCK_STREAM;
    whichIP.ai_protocol = IPPROTO_TCP;
    whichIP.ai_flags = 0;
    
    if(getaddrinfo(argv[1], argv[2], &whichIP, &serverList) == -1){
    	perror("getaddrinfo() error");
    	exit(EXIT_FAILURE);
    }
    
    switch(serverList->ai_family) {
    	case AF_INET:
    		std::cout << "Client with IPv4" << std::endl;
    		break;
    	case AF_INET6:
    		std::cout << "Client with IPv6" << std::endl;
    		break;
    	default:
    		perror("Client with unknown socket");
    		exit(EXIT_FAILURE);
    		break;
    }
    
    int clientfd;
    if ((clientfd = socket(serverList->ai_family, serverList->ai_socktype, serverList->ai_protocol)) == -1){
        perror("socket() error");
        exit(EXIT_FAILURE);
    } 
    
    if (connect(clientfd, serverList->ai_addr, serverList->ai_addrlen) == -1){
        perror("connect() error");
        exit(EXIT_FAILURE);
    } 
    else {
    	char IPaddress[NI_MAXHOST];
    	char portnr[NI_MAXSERV];
    	
    	sockaddr_storage server;
    	server.ss_family = serverList->ai_family;
    	socklen_t server_addrlen;
    	server_addrlen = sizeof(server);
    	
    	getsockname(clientfd, (struct sockaddr *) &server, &server_addrlen);
    	getnameinfo((struct sockaddr *) &server, server_addrlen, IPaddress, NI_MAXHOST, portnr, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);
    	
    	char buff[256];
    	memset(buff, 0, sizeof(server));
    	
    	if(recv(clientfd, buff, sizeof(buff), 0) == -1){
        	perror("recv() error");
        	exit(EXIT_FAILURE);
		} else {
			std::cout << "[CLIENT] Received from server: " << buff << std::endl;
			std::cout << "[CLIENT] Closing..." << std::endl;
			sleep(1);

			close(clientfd);

			exit(EXIT_SUCCESS);
		}
    } 
}
