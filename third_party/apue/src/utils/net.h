#ifndef APUE_NET_H
#define APUE_NET_H

#include <sys/socket.h>
// Internet address family.
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string.h>

#include <netdb.h>

// inet_ntop
// inet_pton

// INET_ADDRSTRLEN
// INET6_ADDRSTRLEN
// struct in_addr
// struct sockaddr
// struct sockaddr_in
// inet_ntop
// inet_pton

int get_interface_addr(struct in_addr *addr);
int resolve_host(const char *node, const char *service, struct sockaddr_in *addr);
int look_up_protocol(const char *name, int *protocol);

char *inet_ip(struct in_addr *addr);

#define SERVER_PORT     8081
#define CLIENT_PORT     8082

#define SERVER_OPTION   "--server"
#define CLIENT_OPTION   "--client"

#define SA struct sockaddr;

#define TCP_NO  6

static inline int choose_process_main_routing(int argc, char **argv,
    void (*server_routing)(int, char **), void (*client_routing)(int, char **)) {
    if (argc < 2) {
       return -1;
    }

    if (!strcmp(SERVER_OPTION, argv[1])) {
        server_routing(argc, argv);
    } else if (!strcmp(CLIENT_OPTION, argv[1])) {
        client_routing(argc, argv);
    } else {
        return -1;
    }

    return 0;
}

char *sock_ntop(struct sockaddr *addr, socklen_t len);

#endif
