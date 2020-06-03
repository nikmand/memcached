//
// Created by nikmand on 3/6/2020.
//

#include "communication.h"

int sockfd;
char buffer[1024] = {0};
struct sockaddr_in address;
int addrlen = sizeof(address);

void initCommunication(){
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; //inet_addr("127.0.0.1");
    address.sin_port = htons(PORT);
    // Forcefully attaching socket to the port 8080
    if (bind(sockfd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 5) < 0) { // maximum number of clients waiting to the queue 5, any new requests gets rejected
        perror("listen");
        exit(EXIT_FAILURE);
    }
}

char rlAgentCommand(){

}


