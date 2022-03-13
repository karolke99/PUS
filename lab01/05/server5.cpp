/*
 * Data:                2022-03-13
 * Autor:               Magdalena Żołnierek & Karol Waligóra
 * Kompilacja:          $ g++ -o server5 server5.cpp -pthread
 * Uruchamianie:        $ ./server4 <numer portu> 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <unistd.h>     
#include <string.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <fstream>

#define MAX_BUFF 30000
#define MAX_RESP 35000

using namespace std;

int initServer(int port);
void* threadFunc(void* new_fd);
void generate_html();

DIR* dir;
char response[512];

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

    //generate root request content
    generate_html();

    while(true) {
        if ((new_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_addr_len)) < 0) {
            perror("accept() error");
            exit(EXIT_FAILURE);
        }

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
    
    //read files and generate response content
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
    } else {
        
        //save filename
        char filename[256];
        strcpy(filename, "img/");
        strcat(filename, url+1);
        filename[strlen(filename)] = '\0';

        //find file extension
        string fileext = string(url);
        int pos = fileext.find_last_of(".");
        fileext = fileext.substr(pos+1);

        if(fileext == "jpg") {
            fileext = "jpeg";
        }

        //open image file
        std::ifstream stream;
        stream.open(filename, std::ifstream::binary);

        if(stream.is_open()) {
            char buffer[MAX_BUFF];
            stream.read(buffer, sizeof(char) * MAX_BUFF);
            int imageSize = (int)stream.gcount();

            char resp[MAX_RESP];

            string sHeader = "HTTP/1.0 200 OK \r\nContent-Type: image/" + fileext + "\r\nContent-Transfer-Encoding: binary\r\nContent-Length: " + std::to_string(imageSize) + "\r\n\n";
            int sHeaderSize = (int)sHeader.size();

            for (int i=0 ; i < sHeaderSize; i++ ) {
                resp[i] = sHeader[i];
            }

            for (int i = sHeaderSize; i < sHeaderSize + imageSize && i < MAX_BUFF; i++){
                resp[i] = buffer[i - sHeaderSize];
            }
            
            if (write(fd, resp, sizeof(resp))== -1){
                perror("write() error");
                exit(EXIT_FAILURE);
            }

            stream.close();

            memset(&buffer, 0, sizeof(buffer));
            memset(&resp, 0, sizeof(resp));
        } else {
            if(write(fd, response, sizeof(response)) == -1){
                perror("write() error");
                exit(EXIT_FAILURE);
            }
        }
    }

    return 0;
}