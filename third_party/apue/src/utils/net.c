#include "utils/net.h"

#include <string.h>

#define LOOP_IP_ADDR   "127.0.0.1"

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
