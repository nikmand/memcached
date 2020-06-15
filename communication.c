//
// Created by nikmand on 3/6/2020.
//

#include "communication.h"

int sockfd, client_socket, read_size;
char buffer[1024] = {0};
struct sockaddr_in address;
int addrlen = sizeof(address);
// SO_REUSEADDR

int initCommunication() {

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // in order to avoid error: 'Port already in use'
    int enable = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 2) < 0) { // maximum number of clients waiting to the queue 5, any new requests gets rejected
        perror("listen");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}

int rlAgentSync(int sockfd){

    if ((client_socket = accept(sockfd, (struct sockaddr *) &address, (socklen_t *) &addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // because comminication is sychronous we respond right away to the correct client using client_socket
    if ((read_size = read(client_socket, buffer, sizeof(buffer))) < 0) {
        perror("read from remote peer failed");
    }

    return client_socket;
}

void sendStats(int client_socket, double q95){

    send(client_socket, &q95, sizeof q95, 0);

    if (close(client_socket) < 0) { // closing the tcp connection
        perror("close");
    }
}

void closeCommunication(){

}


