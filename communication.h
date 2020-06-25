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

#define PORT 8080

int initCommunication();
int rlAgentSync(int sockfd);
void sendStats(int client_socket, double q95);

#endif //MEMCACHED_CLIENT_COMMUNICATION_H
