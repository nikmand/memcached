//
// Created by nikmand on 3/6/2020.
//

#ifndef MEMCACHED_CLIENT_COMMUNICATION_H
#define MEMCACHED_CLIENT_COMMUNICATION_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define PORT 42171

struct stats{
    double q95;
    double rps;
};

int initCommunication();
int rlAgentSync(int sockfd);
void sendStats(int client_socket, struct stats q95);

#endif //MEMCACHED_CLIENT_COMMUNICATION_H
