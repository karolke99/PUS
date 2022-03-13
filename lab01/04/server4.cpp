/*
 * Data:                2022-03-06
 * Autor:               Magdalena Żołnierek & Karol Waligóra
 * Kompilacja:          $ g++ -o server4 server4.cpp
 * Uruchamianie:        $ ./server4 <numer portu> 
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace std;

int server_fd;
fd_set current_sockets;
std::vector<int>clients;
int maxDescriptor;

void newConnection();
void checkClients();
void initServer(int port);

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Invocation: %s <PORT>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int server_port = atoi(argv[1]);
    
    initServer(server_port);

    while(1) {

        FD_ZERO(&current_sockets);
        FD_SET(server_fd, &current_sockets);
        maxDescriptor = server_fd;

        for(auto client : clients) {
            FD_SET(client, &current_sockets);
            if (client > maxDescriptor) {
                maxDescriptor = client;
            }
        }

        if (select(maxDescriptor+1, &current_sockets, NULL, NULL, NULL) < 0) {
            perror("[SERVER] select() error");
            exit(EXIT_FAILURE);
        }

        newConnection();

        checkClients();

    }

    return EXIT_SUCCESS;
}

void initServer(int port) {

    struct sockaddr_in server_addr;

    socklen_t addr_len = sizeof(struct sockaddr_in);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
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
}

void newConnection() {
    int new_socket;
    sockaddr_in client_addr;
    socklen_t client_addr_len;

    memset(&client_addr, 0, sizeof(client_addr));

    if (FD_ISSET(server_fd, &current_sockets)) {

        if ((new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len)) < 0) {
            perror("accept error()");
            exit(EXIT_FAILURE);
        }

        cout << "[SERVER] New connection:" << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;

        const char *welcome="Hello from server!\n";
        if(send(new_socket, welcome, strlen(welcome), 0) != strlen(welcome)) {
            perror("send() error");
            exit(EXIT_FAILURE);
        }

        clients.push_back(new_socket);
        cout << "[SERVER] Number of connected clients: " << clients.size() << endl;

    }
}

void checkClients() {
    sockaddr_in client_addr;
    socklen_t client_addr_len;
    memset(&client_addr, 0, sizeof(client_addr));

    char message[256];
    memset(message, 0, sizeof(message));

    for(auto client : clients) {
        //check if clients is ready
        if (FD_ISSET(client, &current_sockets)) {

            if(read(client, message, sizeof(message)) == 0) {
                getpeername(client, (struct sockaddr*) &client_addr, &client_addr_len);
                cout << "[SERVER] Client disconnected: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << endl;

                close(client);
                FD_CLR(client, &current_sockets);
                clients.erase(find(clients.begin(), clients.end(), client));
            } else {
                for(auto element : clients) {
                    if(element != client) {
                        send(element, message, strlen(message), 0);
                    }
                }
            }
        }
    }
}
