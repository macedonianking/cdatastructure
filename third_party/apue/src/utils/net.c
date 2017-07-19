#include "utils/net.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/apue.h"

#define LOOP_IP_ADDR   "127.0.0.1"

static char ip_addr[INET6_ADDRSTRLEN];

#define SOCK_NTOP_SIZE  1024
static char sock_ntop_buf[SOCK_NTOP_SIZE];

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

char *sock_ntop(struct sockaddr *addr, socklen_t len) {
    int n;
    char *buf;

    buf = sock_ntop_buf;
    switch(addr->sa_family) {
        case AF_INET: {
            struct sockaddr_in *in_addr;

            in_addr = (struct sockaddr_in*) addr;
            if (inet_ntop(AF_INET, &in_addr->sin_addr, buf, SOCK_NTOP_SIZE) == buf) {
                n = strlen(buf);
                if (in_addr->sin_port != 0 && n < SOCK_NTOP_SIZE) {
                    snprintf(buf + n, SOCK_NTOP_SIZE - n, ":%d",
                        ntohs(in_addr->sin_port));
                }
                goto out;
            } else {
                goto meet_error;
            }
        }
    }

meet_error:
    buf[0] = '\0';

out:
    return buf;
}

int say_daytime(int fd) {
    char buf[BUFSIZ];
    time_t seconds;
    int n, n_wr;

    time(&seconds);
    ctime_r(&seconds, buf);
    n = strlen(buf);
    n_wr = writen(fd, buf, n);
    return n_wr == n ? 0 : -1;
}

int create_connection(const struct sockaddr_in *addr) {
    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) != -1) {
        if (connect(fd, (struct sockaddr*) addr, sizeof(*addr))) {
            close(fd);
            fd = -1;
        }
    }
    return fd;
}

int create_local_connection(in_port_t port) {
    struct sockaddr_in addr;
    int fd;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    fd = -1;
    if (inet_pton(AF_INET, LOCAL_IP_ADDR, &addr.sin_addr) == 1) {
        fd = create_connection(&addr);
    }
    return fd;
}

int create_server_socket(struct sockaddr_in *addr, int backlog) {
    int fd;

    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        goto out;
    }
    if (bind(fd, (struct sockaddr*) addr, sizeof(*addr))) {
        goto meet_error;
    }
    if (listen(fd, backlog)) {
        goto meet_error;
    }
out:
    return fd;

meet_error:
    close(fd);
    fd = -1;
    goto out;
}

int get_local_socket_addr(struct sockaddr_in *addr, in_port_t host_port) {
    int r;

    r = -1;
    addr->sin_port = htons(host_port);
    if (inet_pton(AF_INET, LOCAL_IP_ADDR, &addr->sin_addr) == 1) {
        r = 0;
    }
    return r;
}
