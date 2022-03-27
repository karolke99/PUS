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
#include <sys/wait.h>
#include <netinet/ip_icmp.h>
#include <time.h>
#include "checksum.h"

#define MESSAGE_SIZE 32
#define ECHO_MESS_COUNT 4

void echo(const char* destination, int childpid);
void response();

int intN(int n) { return rand() % n; }
char *random_string(int len) {
    srand(time(NULL));
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    char *rstr = malloc((len + 1) * sizeof(char));
    for (int i = 0; i < len; i++) {
        rstr[i] = alphabet[intN(strlen(alphabet))];
    }
    rstr[len] = '\0';
    return rstr;
}


int main(int argc, char** argv) {

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <IP_ADDRESS or HOSTNAME>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int childpid;

    if ((childpid = fork()) == 0) {
        response();
    } else {
        echo(argv[1], childpid);
    }

    wait(NULL);

    exit(EXIT_SUCCESS);

}

void echo(const char* destination, int childpid) {
    
    int sockfd; /* Deskryptor gniazda. */
    int ttl = 128; /* Time to live - czas życia pakietu */
    
    /* Nagłówek ICMP */
    struct icmphdr *icmp_header;

    /* Struktura zawierajaca wskazowki dla funkcji getaddrinfo(): */
    struct addrinfo hints;

    /*
    * Wskaznik na liste zwracana przez getaddrinfo() oraz wskaznik uzywany do
    * poruszania sie po elementach listy:
    */
    struct addrinfo *rp, *result;

    /* Bufor na nagłówek i wiadomość ICMP */
    char datagram[sizeof(struct icmphdr) + MESSAGE_SIZE];
    
    /* Wartość zwracana przez funkcję getaddrinfo() */
    int retval;

    /* Wskaźnik na wiadomość w datagramie ICMP */
    char *message;

    /* Przypisanie wartości do icmp_header oraz message */
    icmp_header = (struct icmphdr*) datagram;
    message = datagram + sizeof(struct icmphdr);

    /* Wskazowki dla getaddrinfo() */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_RAW;
    hints.ai_protocol = IPPROTO_ICMP;


    retval = getaddrinfo(destination, NULL, &hints, &result);
    if (retval != 0) {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(retval));
        kill(childpid, SIGKILL);    //ubicie procesu potomnego
        exit(EXIT_FAILURE);
    }

    /* Przechodzimy kolejno przez elementy listy */
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        /* Utworzenie gniazda dla protokolu ICMP: */
        if ((sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1) {
            perror("socket() error");
            continue;
        }

        retval = setsockopt(
                     sockfd,
                     IPPROTO_IP, IP_TTL,
                     &ttl,
                     sizeof(ttl)
                 );

        if (retval == -1) {
            perror("setsockopt()");
            kill(childpid, SIGKILL);    //ubicie procesu potomnego
            exit(EXIT_FAILURE);
        } else {
            /* Jezeli gniazdo zostalo poprawnie utworzone i
             * opcja TTL ustawiona: */
            break;
        }
    }

    /* Jezeli lista jest pusta (nie utworzono gniazda): */
    if (rp == NULL) {
        fprintf(stderr, "Client failure: could not create socket.\n");
         kill(childpid, SIGKILL);    //ubicie procesu potomnego
        exit(EXIT_FAILURE);
    }

    char* mess_temp = random_string(MESSAGE_SIZE);
    strcpy(message, mess_temp);
    free(mess_temp);

    printf("Message: %s\n", message);

    sleep(1);

    /* Wysylanie ICMP Echo co 1 sekunde: */
    for(int i = 1; i <= ECHO_MESS_COUNT; i++){

        sleep(1);

        /* Nagłówek ICMP ECHO */
        memset(icmp_header, 0, sizeof(struct icmphdr));
        icmp_header->type = ICMP_ECHO;
        icmp_header->code = 0;
        icmp_header->un.echo.id = htons((uint16_t) getpid());
        icmp_header->un.echo.sequence = htons((uint16_t) i);
        icmp_header->checksum = internet_checksum(
                                            (unsigned short *)datagram,
                                            sizeof(struct icmphdr) + MESSAGE_SIZE
                                            );


        
        retval = (int) sendto(
                     sockfd,
                     datagram,
                     sizeof(datagram),
                     0,
                     rp->ai_addr, rp->ai_addrlen
                 );
        
        if (retval == -1) {
            perror("sendto()");
        }
    }
    freeaddrinfo(result);
    close(sockfd);
}

void response() {
    int sockfd; /* Deskryptor gniazda. */
    struct sockaddr addr; /* Adres odpowiedzi */

    /* Nagłówek ICMP */
    struct icmphdr *icmp_header;

    /* Nagłówek IP */
    struct ip *ip_header;

    /* Bufor na naglowek IP, naglowek ICMP oraz wiadomość: */
    char datagram[sizeof(struct ip) + sizeof(struct icmphdr) + MESSAGE_SIZE];

    /* Wskaźnik na wiadomość w datagramie ICMP */
    char *message;

    ip_header = (struct ip*) datagram;
    message = datagram + sizeof(struct ip) + sizeof(struct icmphdr);
    icmp_header = (struct icmphdr*)(datagram + sizeof(struct ip));
    

    sleep(1);

    for(int i = 1; i <= ECHO_MESS_COUNT; i++) {
        
        memset(datagram, 0, sizeof(datagram));

        socklen_t addr_len = sizeof(struct sockaddr);

        sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

        recvfrom(sockfd, &datagram, sizeof(datagram), 0, &addr, &addr_len);

        printf("PING %s\n", inet_ntoa(ip_header->ip_src));
        printf("[%s] received: icmp_seq=%d ttl=%d header_length=%d type=%d code=%d id=%d \n\tmessage=%s\n=================================================================================\n",
                    inet_ntoa(ip_header->ip_dst),
                    ntohs(icmp_header->un.echo.sequence),
                    ip_header->ip_ttl,
                    ip_header->ip_hl,
                    icmp_header->type,
                    icmp_header->code,
                    ntohs(icmp_header->un.echo.id),
                    message
                    );
    }

    
    close(sockfd);
    
}   
