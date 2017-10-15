#include "chapter11/dns_resolve.h"

#define LOG_TAB "dns_resolve"
#include "utils/apue.h"
#include "utils/net.h"
#include "utils/utils.h"

typedef struct s_host_addr_node {
    list_head node;
    struct sockaddr_in addr;
} s_host_addr_node;

void dns_resolve_main(int argc, char **argv) {
    dns_resolve_main_4(argc, argv);
}

static int dns_query_host_ip_addr_list(const char *name, list_head *list) {
    struct hostent ent, *result;
    int h_error;
    char buf[MAXLINE];
    s_host_addr_node *ptr;
    int r;

    r = -1;
    if (!gethostbyname_r(name, &ent, buf, MAXLINE, &result, &h_error)
        && result == &ent
        && result->h_addrtype == AF_INET) {
        for (int i = 0; result->h_addr_list[i]; ++i) {
            ptr = (s_host_addr_node*) malloc(sizeof(s_host_addr_node));
            INIT_LIST_HEAD(&ptr->node);
            memset(&ptr->addr, 0, sizeof(ptr->addr));
            ptr->addr.sin_family = AF_INET;
            ptr->addr.sin_addr = *(struct in_addr*) result->h_addr_list[i];
            list_add(&ptr->node, list);
        }
        r = 0;
    }
    return r;
}

void dns_resolve_main_1(int argc, char **argv) {
    DEFINE_LIST_HEAD(list);
    s_host_addr_node *ptr;
    char ip_addr[INET6_ADDRSTRLEN];

    if (argc < 2) {
        return;
    }
    if (dns_query_host_ip_addr_list(argv[1], &list)) {
        return;
    }
    LIST_FOR_EACH_ENTRY(ptr, &list, node) {
        if (inet_ntop(ptr->addr.sin_family, &ptr->addr.sin_addr, ip_addr, INET6_ADDRSTRLEN) == ip_addr) {
            fprintf(stdout, "%s\n", ip_addr);
        }
    }
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &list, node) {
        list_del(&ptr->node);
        free(ptr);
    }
}

static int get_conn_addr(const char *name, const char *service, struct sockaddr_in *addr) {
    char buf[MAXLINE];
    struct hostent hostent_item, *hostent_result;
    struct servent servent_item, *servent_result;
    int h_error;
    int r;

    r = -1;
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    if (!gethostbyname_r(name, &hostent_item, buf, MAXLINE, &hostent_result, &h_error)
        && hostent_result == &hostent_item
        && hostent_result->h_addrtype == AF_INET
        && hostent_result->h_addr_list[0]) {
        addr->sin_addr = *(struct in_addr*) hostent_result->h_addr_list[0];
        if (!getservbyname_r(service, "tcp", &servent_item, buf, MAXLINE, &servent_result)) {
            r = 0;
            addr->sin_port = servent_result->s_port;
        }
    }
    return r;
}

void dns_resolve_main_2(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;

    if (argc < 3 || get_conn_addr(argv[1], argv[2], &addr)) {
        return;
    }
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
    }
    if (connect(fd, (SA*) &addr, sizeof(addr)) == -1) {
        close(fd);
        return;
    }
    ALOGE("connect success");
    close(fd);
    return;
}

void dns_resolve_main_3(int argc, char **argv) {
    struct addrinfo hints, *result, *ptr;
    char ip_buf[INET6_ADDRSTRLEN];
    int r;

    bzero(&hints, sizeof(hints));
    hints.ai_flags = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ((r = getaddrinfo(NULL, "https", &hints, &result)) || !result) {
        ALOGE("getaddrinfo failure: %s", gai_strerror(r));
        return;
    }
    for (ptr = result; ptr; ptr = ptr->ai_next) {
        void *addr = NULL;
        switch (ptr->ai_family) {
            case AF_INET: {
                addr = &((struct sockaddr_in*)ptr->ai_addr)->sin_addr;
                break;
            }
            case AF_INET6:{
                addr = &((struct sockaddr_in6*)ptr->ai_addr)->sin6_addr;
                break;
            }
        }
        if (addr && inet_ntop(ptr->ai_family, addr, ip_buf, INET6_ADDRSTRLEN)) {
            ALOGD("%s", ip_buf);
        }
    }
    freeaddrinfo(result);
}

void dns_resolve_main_4(int argc, char **argv) {
    uint16_t port;
    int fd;

    if (dns_service_port("http", "tcp", &port)) {
        return;
    }
    if ((fd = tcp_connect_port("www.ifeng.com", port)) == -1) {
        return;
    }
    close(fd);
    ALOGD("success: port=%d", port);
}
