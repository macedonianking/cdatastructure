#include "chapter11/dns_resolve.h"

#define LOG_TAB "dns_resolve"
#include "utils/apue.h"
#include "utils/net.h"
#include "utils/utils.h"

void dns_resolve_main(int argc, char **argv) {
    dns_resolve_main_4(argc, argv);
}

void dns_resolve_main_1(int argc, char **argv) {
    char *host, buf[MAXLINE], ip_buf[INET6_ADDRSTRLEN];
    struct hostent ent, *ptr;
    struct sockaddr_in **addr_list, *addr;
    int herror, i;

    if (argc != 2) {
        return;
    }

    host = argv[1];
    if (gethostbyname_r(host, &ent, buf, MAXLINE, &ptr, &herror) ||
        !ptr || ptr->h_addrtype != AF_INET) {
        return;
    }
    fprintf(stdout, "name: %s\n", ptr->h_name);
    fprintf(stdout, "type: AF_INET\n");
    addr_list = (struct sockaddr_in**) ptr->h_addr_list;
    i = 0;
    for (;(addr = *addr_list); ++addr_list) {
        if (addr->sin_addr.s_addr &&
            inet_ntop(AF_INET, &addr->sin_addr, ip_buf, INET6_ADDRSTRLEN) == ip_buf) {
            fprintf(stdout, "addr%d: %s:%d\n", ++i, ip_buf, ntohs(addr->sin_port));
        }
    }
}

/**
 * 输出每个服务项
 */
static void print_service_entry(struct servent *ptr) {
    char **aliases;
    if (!ptr) {
        return;
    }

    fprintf(stdout, "name: %s\n", ptr->s_name);
    if (ptr->s_aliases && *ptr->s_aliases) {
        aliases = ptr->s_aliases;
        fprintf(stdout, "aliases:");
        while (*aliases) {
            fprintf(stdout, " %s", *aliases);
            ++aliases;
        }
        fprintf(stdout, "\n");
    }
    fprintf(stdout, "port: %d\n", ntohs(ptr->s_port));
    fprintf(stdout, "protocol: %s\n\n", ptr->s_proto);
}

void dns_resolve_main_2(int argc, char **argv) {
    struct servent *ent;

    setservent(1);
    while ((ent = getservent()) != NULL) {
        print_service_entry(ent);
    }
    endservent();

    if ((ent = getservbyname("https", "tcp"))) {
        print_service_entry(ent);
    }
}

// 输出站点信息
static void print_addr_info_entry(struct addrinfo *ptr, const char *node) {
    char ip_addr[INET6_ADDRSTRLEN];
    struct sockaddr_in *addr;

    if (!ptr || ptr->ai_family != AF_INET) {
        return;
    }
    addr = (struct sockaddr_in*) ptr->ai_addr;
    fprintf(stdout, "name: %s\n", node);
    fprintf(stdout, "famlily: AF_INET\n");
    fprintf(stdout, "socket type: %d\n", ptr->ai_socktype);
    fprintf(stdout, "protocol: %d\n", ptr->ai_protocol);
    fprintf(stdout, "addr: ");
    if (inet_ntop(addr->sin_family, &addr->sin_addr, ip_addr, INET6_ADDRSTRLEN) == ip_addr) {
        fprintf(stdout, "%s: %d", ip_addr, ntohs(addr->sin_port));
    }
    fprintf(stdout, "\n\n");
}

void dns_resolve_main_3(int argc, char **argv) {
    struct addrinfo hint, *src, *ptr;
    char *name;

    // Initilize hint strcuture.
    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME | AI_PASSIVE;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = IPPROTO_TCP;

    // 获取信息
    if (!getaddrinfo("www.baidu.com", "http", &hint, &src)) {
        for (ptr = src; ptr; ptr = ptr->ai_next) {
            if (ptr->ai_canonname != NULL) {
                name = ptr->ai_canonname;
            }
            print_addr_info_entry(ptr, name);
        }
        freeaddrinfo(src);
    }
}

void dns_resolve_main_4(int argc, char **argv) {
    int fd;

    fd = tcp_connect("www.baidu.com", "http");
    ALOGE("fd=%d", fd);
    if (fd != -1) {
        close(fd);
    }
}
