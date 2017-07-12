#ifndef APUE_NET_H
#define APUE_NET_H

#include <sys/socket.h>
// Internet address family.
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <netdb.h>

// inet_ntop
// inet_pton

// INET_ADDRSTRLEN
// INET6_ADDRSTRLEN

int get_interface_addr(struct in_addr *addr);

#define SERVER_PORT     8081
#define CLIENT_PORT     8082

#endif
