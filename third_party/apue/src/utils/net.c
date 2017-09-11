#include "utils/net.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils/apue.h"

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

int get_loopback_addr(struct in_addr *addr) {
    int r;
    struct addrinfo hints, *src, *ptr;

    r = -1;
    bzero(&hints, sizeof(hints));
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if (!getaddrinfo("loopback", "http", &hints, &src) && src) {
        for (ptr = src; ptr; ptr = ptr->ai_next) {
            if (ptr->ai_family == AF_INET && addr) {
                *addr = ((struct sockaddr_in*) ptr->ai_addr)->sin_addr;
                r = 0;
                break;
            }
        }
        freeaddrinfo(src);
    }

    return r;
}


int resolve_host(const char *node, const char *service, struct sockaddr_in *addr) {
    struct addrinfo hint, *ret;

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_protocol = IPPROTO_TCP;
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

char *sock_ntop(struct sockaddr *addr, socklen_t len, char *buf, int size) {
    int n;
    uint16_t port;

    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *impl = (struct sockaddr_in*)addr;
        if (inet_ntop(impl->sin_family, &impl->sin_addr, buf, size) == buf) {
            if ((port = ntohs(impl->sin_port)) != 0) {
                n = strlen(buf);
                snprintf(buf + n, size - n, ":%u", port);
            }
            return buf;
        }
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *impl = (struct sockaddr_in6*) addr;
        if (inet_ntop(impl->sin6_family, &impl->sin6_addr, buf, size) == buf) {
            if ((port = ntohs(impl->sin6_port)) != 0) {
                n = strlen(buf);
                snprintf(buf + n, size - n, ":%u", port);
            }
            return buf;
        }
    }
    return NULL;
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
    int old_errno;

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
    old_errno = errno;
    close(fd);
    errno = old_errno;
    fd = -1;
    goto out;
}

int get_local_socket_addr(struct sockaddr_in *addr, in_port_t host_port) {
    int r;

    r = -1;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(host_port);
    if (inet_pton(AF_INET, LOCAL_IP_ADDR, &addr->sin_addr) == 1) {
        r = 0;
    }
    return r;
}

struct addrinfo *host_serv(const char *node, const char *service, int family, int socktype) {
    struct addrinfo hint, *ptr;

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = family;
    hint.ai_socktype = socktype;

    if (getaddrinfo(node, service, &hint, &ptr) != 0) {
        ptr = NULL;
    }
    return ptr;
}

/**
 * 创建tcp的socket并且实行链接
 */
int tcp_connect(const char *node, const char *service) {
    struct addrinfo *src, *ptr;
    int fd, temp_fd;

    fd = -1;
    src = host_serv(node, service, AF_UNSPEC, SOCK_STREAM);
    for (ptr = src; ptr; ptr = ptr->ai_next) {
        if ((temp_fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
            continue;
        }
        if (!connect(temp_fd, ptr->ai_addr, ptr->ai_addrlen)) {
            fd = temp_fd;
            break;
        } else {
            close(fd);
        }
    }
    if (src) {
        freeaddrinfo(src);
    }

    return fd;
}

int tcp_listen(const char *service, socklen_t *addrlen) {
    struct addrinfo hint, *src, *ptr;
    int fd;
    int on;

    bzero(&hint, sizeof(hint));
    hint.ai_flags = AI_PASSIVE;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_family = AF_UNSPEC;

    if (getaddrinfo(NULL, service, &hint, &src) || !src) {
        return -1;
    }

    fd = -1;
    for (ptr = src; ptr; ptr = ptr->ai_next) {
        if ((fd = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol)) == -1) {
            continue;
        }
        on = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) {
            close(fd);
            fd = -1;
            continue;
        }
        if (bind(fd, ptr->ai_addr, ptr->ai_addrlen) || listen(fd, LISTEN_BACKLOG_COUNT)) {
            close(fd);
            fd = -1;
            continue;
        }
        break;
    }
    if (src) {
        freeaddrinfo(src);
    }
    return fd;
}

int fill_socket_addr(struct sockaddr_storage *addr, const char *src) {
    bzero(addr, sizeof(*addr));
    if (inet_pton(AF_INET6, src, &((struct sockaddr_in6*)addr)->sin6_addr) == 1) {
        addr->ss_family = AF_INET;
        return 0;
    }
    if (inet_pton(AF_INET, src, &((struct sockaddr_in*)addr)->sin_addr) == 1) {
        addr->ss_family = AF_INET;
        return 0;
    }
    return -1;
}

int tcp_bind_wildcard(int domain, uint16_t port) {
    int fd;
    struct sockaddr *bind_addr;
    socklen_t length;

    if (domain != AF_INET && domain != AF_INET6) {
        return -1;
    }
    if ((fd = socket(domain, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        return fd;
    }
    if (domain == AF_INET) {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = domain;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);

        bind_addr = (SA*) &addr;
        length = sizeof(addr);
    } else if (domain == AF_INET6) {
        struct sockaddr_in6 addr;

        bzero(&addr, sizeof(addr));
        addr.sin6_family = domain;
        addr.sin6_addr = in6addr_any;
        addr.sin6_port = htons(port);

        bind_addr = (SA*) &addr;
        length = sizeof(addr);
    }

    if (bind(fd, bind_addr, length)) {
        close(fd);
        fd = -1;
    }
    return fd;
}

int tcp_listen_wildcard(int domain, uint16_t port) {
    int fd = tcp_bind_wildcard(domain, port);
    if (fd != -1) {
        if (listen(fd, LISTEN_BACKLOG_COUNT) == -1) {
            close(fd);
            fd = -1;
        }
    }
    return fd;
}
