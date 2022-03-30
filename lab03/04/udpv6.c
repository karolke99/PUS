#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef __USE_BSD
#define __USE_BSD
#endif

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <error.h>

#define SOURCE_PORT 5050

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(
            stderr,
            "Invocation: %s <IPv6> <PORT>\n",
            argv[0]
        );

        exit(EXIT_FAILURE);
    }

    /* Struktura zawierajaca wskazowki dla funkcji getaddrinfo(): */
    struct addrinfo hints;
    int retval;
    int sockfd;
    
    /*
    * Wskaznik na liste zwracana przez getaddrinfo() oraz wskaznik uzywany do
    * poruszania sie po elementach listy:
    */
    struct addrinfo *rp, *result;

    int offset = 6;

    /* Bufor na naglowek UDP: */
    unsigned char datagram[sizeof(struct udphdr)] = {0};

    struct udphdr *udp_header = (struct udphdr *) datagram;
    

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_UDP;

    if ((retval = getaddrinfo(argv[1], NULL, &hints, &result)) != 0){
        perror("getaddrinfor() error");
        exit(EXIT_FAILURE);
    }

    /* Przechodzimy kolejno przez elementy listy: */
    for (rp = result; rp != NULL; rp = rp->ai_next) {

        /* Utworzenie gniazda dla protokolu UDP: */
        if ((sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            perror("socket()");
            continue;
        }

        retval = setsockopt(
                     sockfd,
                     IPPROTO_IPV6, IPV6_CHECKSUM,
                     &offset,
                     sizeof(offset)
                 );

        if (retval == -1) {
            perror("setsockopt()");
            exit(EXIT_FAILURE);
        } else {
            break;
        }
    }

    /* Jezeli lista jest pusta (nie utworzono gniazda): */
    if (rp == NULL) {
        fprintf(stderr, "Client failure: could not create socket.\n");
        exit(EXIT_FAILURE);
    }
    
    /* Za wypełnienie pól nagłówka IP odpowiedzialny jest system oepracyjny */

    udp_header->uh_sport = htons(SOURCE_PORT);
    udp_header->uh_dport = htons(atoi(argv[2]));
    udp_header->uh_ulen = htons(sizeof(struct udphdr));

    /* Wysylanie datagramow co 1 sekunde: */
    for (;;) {

        fprintf(stdout, "Sending UDP...\n");

        retval = sendto(
                     sockfd,
                     datagram, sizeof(struct udphdr),
                     0,
                     rp->ai_addr, rp->ai_addrlen
                 );

        if (retval == -1) {
            perror("sendto()");
        }

        sleep(1);
    }

    exit(EXIT_SUCCESS);
}