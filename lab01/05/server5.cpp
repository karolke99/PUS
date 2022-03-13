#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> /* socket() */
#include <netinet/in.h> /* struct sockaddr_in */
#include <arpa/inet.h>  /* inet_ntop() */
#include <unistd.h>     /* close() */
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <sys/stat.h>

using namespace std;

int initServer(int port);
void* threadFunc(void* new_fd);
DIR* dir;

char response[512];

void generate_html() {

    strcpy(response, "HTTP/1.0 200 OK \r\n");
    strcat(response, "Content-Type: text/html\r\n");
    strcat(response, "Content-Length: 512\r\n\n");
    strcat(response, "<html><body><center>");

    struct dirent* dr;

    if((dir = opendir("img/")) == NULL ){
        perror("opendir error");
        exit(EXIT_FAILURE);
    }
    
    while((dr = readdir(dir)) != NULL) {
        if(!strcmp(dr->d_name, ".") || !strcmp(dr->d_name, "..")) {
            continue;
        } else if (strstr(dr->d_name, ".jpg") != NULL ||
                    strstr(dr->d_name, ".jpeg") != NULL ||
                    strstr(dr->d_name, ".png") != NULL ||
                    strstr(dr->d_name, ".gif") != NULL)
        {
            strcat(response, "<img src='");
            strcat(response, dr->d_name);
            strcat(response, "'><br>");
        } else {
            continue;
        }
    }
    
    strcat(response, "</center></body></html>");
    response[strlen(response)] = '\0';
    closedir(dir);

}

int main(int argc, char* argv[]) {
    int server_fd;
    int new_fd;

    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len;

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_port = atoi(argv[1]);

    server_fd = initServer(server_port);

    generate_html();

    while(true) {
        if ((new_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len)) < 0) {
            perror("accept() error");
            exit(EXIT_FAILURE);
        }
        cout << "[SERVER] New connection." << endl;

        //Client service thread
        pthread_t client_thread;

        
        if (pthread_create(&client_thread, NULL, threadFunc, &new_fd) == -1) {
            perror("pthread_create() error");
            exit(EXIT_FAILURE);
        }

        pthread_join(client_thread, NULL);
        close(new_fd);
    }

    close(server_fd);
    exit(EXIT_SUCCESS);

}

int initServer(int port) {
    int server_fd;
    struct sockaddr_in server_addr;

    socklen_t addr_len = sizeof(struct sockaddr_in);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) == -1){
        perror("[SERVER] socket() error");
        exit(EXIT_FAILURE);
    }else {
        cout << "[SERVER] Socket has been created" << endl;
    };

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) == -1 ) {
        perror("[SERVER] bind() error");
        exit(EXIT_FAILURE);
    } else {
        cout << "[SERVER] Socket has got its name" << endl;
    }

    if(listen(server_fd, SOMAXCONN) == -1) {
        perror("[SERVER] listen() error");
        exit(EXIT_FAILURE);
    } else {
        cout << "[SERVER] Listen for connections on a socket..." << endl;
    }

    return server_fd;
}


void* threadFunc(void* new_fd) {
    int fd = *(int *)new_fd;
    char request[2048];
    char buff[512];
    char method[256];
    char protocol[256];
    char url[256];

    int recived_bytes;

    if((recived_bytes = recv(fd, request, sizeof(request), 0)) <= 0) {
        perror("recv() error");
        exit(EXIT_FAILURE);
    }

    request[recived_bytes]='\0';
    sscanf(request, "%s %s %s", method, url, protocol);
    method[strlen(method)] = '\0';
    url[strlen(url)] = '\0';
    protocol[strlen(protocol)] = '\0';
    cout << method << " " << url << " " << protocol << endl;

    if(strcmp(protocol, "HTTP/1.1") != 0) {
        perror("protocol error");
        exit(EXIT_FAILURE);
    }

    bool isRootRequest;
    if((url[strlen(url) - 1] == '/') && strlen(url) == 1) {
        isRootRequest = 1;
    }else {
        isRootRequest = 0;
    }

    if(isRootRequest) {
        if(write(fd, response, strlen(response)) < 0){
            perror("write() error");
            exit(EXIT_FAILURE);
        }
        cout << response << endl;
    } else {
        char *sendbuf;
        FILE *requested_file;
        
        char resp[2048];

        char filename[256];
        strcpy(filename, "img/");
        strcat(filename, url+1);
        filename[strlen(filename)] = '\0';
        cout << "plik: " << filename <<endl;

        if ((requested_file = fopen(filename, "rb")) == NULL) {
            perror("fopen error()");
            exit(EXIT_FAILURE);
        }
        fseek(requested_file, 0, SEEK_END);
        int fileLength = ftell(requested_file);
        rewind(requested_file);

        sendbuf = (char*)malloc(sizeof(char)*fileLength);
        size_t result = fread(sendbuf, 1, fileLength, requested_file);
        strcpy(resp, "HTTP/1.0 200 OK \r\n");
        strcat(resp, "Content-Type: image/jpeg\r\n");
        strcat(resp, "Content-Transfer-Encoding: binary\r\n");
        strcat(resp, "Content-Length: 30000\r\n\n");
        strcat(resp, "<html><head><title>File</file></head><body>");
        cout << "length: " << fileLength << endl;

        if(result < 0) {
            perror("fread error");
            exit(EXIT_FAILURE);
        }

        strcat(resp, sendbuf);
        strcat(resp, "</body></html>");
        resp[strlen(resp)] = '\0';

        write(fd, resp, result);
        fclose(requested_file);
    }



    return 0;
}