#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <error.h>
#include "checksum.h"

/* Struktura pseudo-naglowka (do obliczania sumy kontrolnej naglowka UDP): */
struct phdr {
    struct in_addr ip_dst;
    unsigned char unused;
    unsigned char protocol;
    unsigned short length;

};


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

    /* Bufor na naglowek UDP oraz pseudo-naglowek: */
    unsigned char datagram[sizeof(struct udphdr) + sizeof(struct phdr)] = {0};

    struct udphdr *udp_header = (struct udphdr *)(datagram);
    struct phdr *pseudo_header = (struct phdr *)(datagram + sizeof(struct udphdr));

    /* Zmienna wykorzystywana do obliczenia sumy kontrolnej: */
    unsigned short checksum;

    

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

    /* Port docelowy (z argumentu wywolania): */
    udp_header->uh_dport = htons(atoi(argv[2]));
    udp_header->uh_ulen = htons(sizeof(struct udphdr));

    pseudo_header->ip_dst.s_addr = ((struct sockaddr_in*)rp->ai_addr)->sin_addr.s_addr;
    pseudo_header->unused = 0;
    pseudo_header->protocol = IPPROTO_UDP;
    pseudo_header->length = udp_header->uh_ulen;
    
    udp_header->uh_sum = 0;

    checksum = internet_checksum((unsigned short *)udp_header,
                                    sizeof(struct udphdr)
                                    + sizeof(struct phdr)
                                );
    
    udp_header->uh_sum = (checksum == 0) ? 0xffff : checksum;

    

    /* Wysylanie datagramow co 1 sekunde: */
    for (;;) {

        fprintf(stdout, "Sending UDP...\n");

        /*
         * Prosze zauwazyc, ze pseudo-naglowek nie jest wysylany
         * (ale jest umieszczony w buforze za naglowkiem UDP dla wygodnego
         * obliczania sumy kontrolnej):
         */
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