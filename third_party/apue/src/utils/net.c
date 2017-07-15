#include "utils/net.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define LOOP_IP_ADDR   "127.0.0.1"

static char ip_addr[INET6_ADDRSTRLEN];

int get_interface_addr(struct in_addr *addr) {
    struct in_addr lo_addr;
    struct ifaddrs *ret, *ptr;
    struct sockaddr_in *sock_addr;
    int r;

    if (inet_pton(AF_INET, LOOP_IP_ADDR, &lo_addr) != 1) {
        return -1;
    }

    r = -1;
    if (!getifaddrs(&ret)) {
        for (ptr = ret; ptr; ptr = ptr->ifa_next) {
            if (ptr->ifa_addr
                && ptr->ifa_addr->sa_family == AF_INET
                && ptr->ifa_name) {
                sock_addr = (struct sockaddr_in*) ptr->ifa_addr;
                if (sock_addr->sin_addr.s_addr != lo_addr.s_addr) {
                    r = 0;
                    addr->s_addr = sock_addr->sin_addr.s_addr;
                    break;
                }
            }
        }
        freeifaddrs(ret);
    }

    return r;
}


int resolve_host(const char *node, const char *service, struct sockaddr_in *addr) {
    struct addrinfo hint, *ret;
    int protocol;

    if (look_up_protocol("tcp", &protocol)) {
        return -1;
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_protocol = protocol;
    hint.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(node, service, &hint, &ret) || !ret) {
        return -1;
    }
    *addr = *(struct sockaddr_in*)ret->ai_addr;
    freeaddrinfo(ret);
    return 0;
}

int look_up_protocol(const char *name, int *protocol) {
    struct protoent ent, *ret;
    char buffer[BUFSIZ];

    if (!getprotobyname_r(name, &ent, buffer, BUFSIZ, &ret) && ret == &ent) {
        if (protocol) {
            *protocol = ent.p_proto;
        }
        return 0;
    }
    return -1;
}

char *inet_ip(struct in_addr *addr) {
    if (inet_ntop(AF_INET, addr, ip_addr, INET6_ADDRSTRLEN) != ip_addr) {
        ip_addr[0] = '\0';
    }
    return ip_addr;
}
