#define _GNU_SOURCE     /* getresgid() */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <sys/capability.h>
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>


int main(int argc, char** argv) {

    /* Deskryptory dla gniazda nasluchujacego i polaczonego: */
    int             listenfd, connfd;

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len, server_addr_len;

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];

    unsigned int    port_number;

    cap_t           capabilities;
    
    cap_value_t cap_list[1]={CAP_NET_BIND_SERVICE};
    
    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    port_number = atoi(argv[1]);

    /* Utworzenie gniazda dla protokolu TCP: */
    listenfd = socket(PF_INET, SOCK_STREAM, 0);
    if (listenfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }

    /* Wyzerowanie struktury adresowej serwera: */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    server_addr.sin_family          =       AF_INET;
    /* Adres nieokreslony (ang. wildcard address): */
    server_addr.sin_addr.s_addr     =       htonl(INADDR_ANY);
    /* Numer portu: */
    server_addr.sin_port            =       htons(port_number);
    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_addr_len                 =       sizeof(server_addr);
    capabilities = cap_get_proc();
    cap_set_flag(capabilities,CAP_EFFECTIVE,1,cap_list,CAP_SET);
    cap_set_proc(capabilities);
    fprintf(stdout, "Binding to port %u...\n", port_number);
    
    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(listenfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }
    
    /* Przeksztalcenie gniazda w gniazdo nasluchujace: */
    if (listen(listenfd, 2) == -1) {
        perror("listen()");
        exit(EXIT_FAILURE);
    }
    
    if(cap_clear(capabilities) == -1) {
    	perror("cap_clear()");
    	exit(EXIT_FAILURE);
    }
    
    if(cap_set_proc(capabilities) == -1) {
    	perror("cap_set_proc()");
    	exit(EXIT_FAILURE);
    }
    
    if(cap_free(capabilities) == -1) {
    	perror("cap_free()");
    	exit(EXIT_FAILURE);
    }
    
    fprintf(stdout, "Server is listening for incoming connection...\n");

    /* Funkcja pobiera polaczenie z kolejki polaczen oczekujacych na zaakceptowanie
     * i zwraca deskryptor dla gniazda polaczonego: */
    client_addr_len = sizeof(client_addr);
    connfd = accept(listenfd, (struct sockaddr*)&client_addr, &client_addr_len);
    if (connfd == -1) {
        perror("accept()");
        exit(EXIT_FAILURE);
    }
;
    fprintf(stdout, "TCP connection accepted from %s:%d\n",
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
            ntohs(client_addr.sin_port)
    );

    sleep(5);

    fprintf(stdout, "Closing connected socket (sending FIN to client)...\n");
    close(connfd);

    close(listenfd);

    exit(EXIT_SUCCESS);
}
