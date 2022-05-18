#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_pton() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <errno.h>
#include <openssl/hmac.h>
#include <openssl/err.h>
#include <openssl/evp.h>

int main(int argc, char** argv) {

    if (argc != 3) {
        fprintf(stderr, "Invocation: %s <IPv4 ADDRESS> <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int             sockfd;                 /* Desktryptor gniazda. */
    int             retval;                 /* Wartosc zwracana przez funkcje. */
    struct          sockaddr_in remote_addr;/* Gniazdowa struktura adresowa. */
    socklen_t       addr_len;               /* Rozmiar struktury w bajtach. */

    char            buff[256] = "Laboratorium PUS.";

    int plaintext_len, ciphertext_len, hmac_message_len;

    unsigned char   ciphertext[1024];
    char   hmac_message[1024];
    char   hmac[16];
    int hmac_len;

    unsigned char keyA[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                           0x00,0x01,0x02,0x03,0x04,0x05
                          };
    unsigned char keyB[] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
                           0x10,0x11,0x12,0x13,0x14,0x15
                          };

    int tmp;

    /* Zaladowanie tekstowych opisow bledow: */
    ERR_load_crypto_strings();

    /*Kontkst HMAC: */
    HMAC_CTX *ctx_hmac;

    /*Kontekst: */
    EVP_CIPHER_CTX *ctx;
    const EVP_CIPHER* cipher;

    // ----------- HMAC ----------------
    printf("Generating HMAC...\n");
    
    /*Alokacja pamięci: */
    ctx_hmac = HMAC_CTX_new();

    /*Wyzerowanie kontekstu: */
    HMAC_CTX_reset(ctx_hmac);

    /*Konfiguracja kontekstu: */
    retval = HMAC_Init_ex(ctx_hmac, keyA, sizeof(keyA), EVP_md5(), NULL);
    if(!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    /*Obliczanie HMAC: */
    plaintext_len = strlen(buff);
    retval = HMAC_Update(ctx_hmac, buff, plaintext_len);
    if(!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    /*Zapisanie HMAC: */
    retval = HMAC_Final(ctx_hmac, hmac, &hmac_len);
    if(!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    printf("HMAC generated.\n");

    /*Zwolnienie informacji z kontekstu: */
    HMAC_CTX_free(ctx_hmac);


    // ---------- CIPHER ----------------

    /*Alokacja pamięci dla kontekstu: */
    ctx = EVP_CIPHER_CTX_new();
    EVP_CIPHER_CTX_init(ctx);
    cipher = EVP_aes_128_ecb();

    strcpy(hmac_message, hmac);
    strcpy(hmac_message + 16, buff);

    printf("Encrypting...\n");
    retval = EVP_EncryptInit_ex(ctx, cipher, NULL, keyB, NULL);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX_set_padding(ctx, 1);   

    hmac_message_len = strlen(hmac_message);

    retval = EVP_EncryptUpdate(ctx, ciphertext, &ciphertext_len,
                                (unsigned char*)hmac_message, hmac_message_len);

    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    retval = EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &tmp);
    if (!retval) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    EVP_CIPHER_CTX_free(ctx);
    ciphertext_len += tmp;

     

    /* Utworzenie gniazda dla protokolu UDP: */
    sockfd = socket(PF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("socket()");
        exit(EXIT_FAILURE);
    }


    /* Wyzerowanie struktury adresowej dla adresu zdalnego (serwera): */
    memset(&remote_addr, 0, sizeof(remote_addr));
    /* Domena komunikacyjna (rodzina protokolow): */
    remote_addr.sin_family = AF_INET;

    /* Konwersja adresu IP z postaci kropkowo-dziesietnej: */
    retval = inet_pton(AF_INET, argv[1], &remote_addr.sin_addr);
    if (retval == 0) {
        fprintf(stderr, "inet_pton(): invalid network address!\n");
        exit(EXIT_FAILURE);
    } else if (retval == -1) {
        perror("inet_pton()");
        exit(EXIT_FAILURE);
    }

    remote_addr.sin_port = htons(atoi(argv[2])); /* Numer portu. */
    addr_len = sizeof(remote_addr); /* Rozmiar struktury adresowej w bajtach. */

    fprintf(stdout, "Sending message to %s.\n", argv[1]);

    /* sendto() wysyla dane na adres okreslony przez strukture 'remote_addr': */
    retval = sendto(
                 sockfd,
                 ciphertext, ciphertext_len,
                 0,
                 (struct sockaddr*)&remote_addr, addr_len
             );

    if (retval == -1) {
        perror("sendto()");
        exit(EXIT_FAILURE);
    }

    close(sockfd);
    exit(EXIT_SUCCESS);
}
