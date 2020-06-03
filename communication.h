//
// Created by nikmand on 3/6/2020.
//

#ifndef MEMCACHED_CLIENT_COMMUNICATION_H
#define MEMCACHED_CLIENT_COMMUNICATION_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void initCommunication();
char rlAgentCommand();
void sendAck();
void sendStats();

#endif //MEMCACHED_CLIENT_COMMUNICATION_H
