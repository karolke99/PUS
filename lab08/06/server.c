#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include <openssl/hmac.h>
#include <openssl/err.h>
#include <openssl/evp.h>

int main(int argc, char** argv) {

    int             sockfd; /* Deskryptor gniazda. */
    int             retval; /* Wartosc zwracana przez funkcje. */

    /* Gniazdowe struktury adresowe (dla klienta i serwera): */
    struct          sockaddr_in client_addr, server_addr;

    /* Rozmiar struktur w bajtach: */
    socklen_t       client_addr_len, server_addr_len;

    /* Bufor wykorzystywany przez recvfrom(): */
    char            buff[256];
    int buff_len;

    /* Bufor dla adresu IP klienta w postaci kropkowo-dziesietnej: */
    char            addr_buff[256];

    /*Bufor szyfrogramu*/
    unsigned char ciphertext[1024];
    int ciphertext_len;

    char hmac_message[1024];
    int hmac_message_len;

    char plaintext[256];

    char hmac[16];
    int hmac_len;

    unsigned char keyA[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                           0x00,0x01,0x02,0x03,0x04,0x05
                          };
    unsigned char keyB[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                           0x10,0x11,0x12,0x13,0x14,0x15
                          };

    int tmp;

    ERR_load_crypto_strings();

    HMAC_CTX *ctx_hmac;

    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER* cipher;
    ctx = EVP_CIPHER_CTX_new();

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    /* Utworzenie gniazda dla protokolu UDP: */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
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
    server_addr.sin_port            =       htons(atoi(argv[1]));
    /* Rozmiar struktury adresowej serwera w bajtach: */
    server_addr_len                 =       sizeof(server_addr);

    /* Powiazanie "nazwy" (adresu IP i numeru portu) z gniazdem: */
    if (bind(sockfd, (struct sockaddr*) &server_addr, server_addr_len) == -1) {
        perror("bind()");
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Server is waiting for UDP datagram...\n");

    client_addr_len = sizeof(client_addr);

    /* Oczekiwanie na dane od klienta: */
    retval = recvfrom(
                 sockfd,
                 ciphertext, sizeof(ciphertext),
                 0,
                 (struct sockaddr*)&client_addr, &client_addr_len
             );
    if (retval == -1) {
        perror("recvfrom()");
        exit(EXIT_FAILURE);
    }

    ciphertext_len = retval;

    fprintf(stdout, "UDP datagram received from %s:%d.\n",
            inet_ntop(AF_INET, &client_addr.sin_addr, addr_buff, sizeof(addr_buff)),
            ntohs(client_addr.sin_port)
           );

    
    cipher = EVP_aes_128_ecb();

    printf("Decrypting...\n");
    retval = EVP_DecryptInit_ex(ctx, cipher, NULL, keyB, NULL);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX_set_padding(ctx, 1);

    retval = EVP_DecryptUpdate(ctx, (unsigned char*)hmac_message, &hmac_message_len,
                                ciphertext, ciphertext_len);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    retval = EVP_DecryptFinal_ex(ctx, (unsigned char*)hmac_message + hmac_message_len, &tmp);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    hmac_message_len += tmp;
    hmac_message[hmac_message_len] = '\0';

    EVP_CIPHER_CTX_free(ctx);
    ERR_free_strings();

    printf("Checking HMAC...\n");
    strcpy(buff, hmac_message+16);
    buff_len = strlen(buff);

    ctx_hmac = HMAC_CTX_new();
    HMAC_CTX_reset(ctx_hmac);

    retval = HMAC_Init_ex(ctx_hmac, keyA, sizeof(keyA), EVP_md5(), NULL);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    retval = HMAC_Update(ctx_hmac, buff, buff_len);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    retval = HMAC_Final(ctx_hmac, hmac, &hmac_len);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    HMAC_CTX_free(ctx_hmac);

    for(int i=0; i < 16; i++) {
        if(hmac[i] != hmac_message[i]) {
            printf("HMAC verification failure. Digest does not match.\n");
            exit(EXIT_FAILURE);
        }
    }

    printf("HMAC verified successfully.\n");

    fprintf(stdout, "Message: ");
    fwrite(buff, buff_len, retval, stdout);
    fprintf(stdout, "\n");

    close(sockfd);
    exit(EXIT_SUCCESS);
}
