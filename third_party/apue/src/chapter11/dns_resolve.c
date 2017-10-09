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

typedef struct tcp_service_entry {
    list_head   node;
    char        *service_name;
    uint16_t    port;
} tcp_service_entry;

tcp_service_entry *create_tcp_service_entry(struct servent *src) {
    tcp_service_entry *ptr;

    ptr = (tcp_service_entry*) malloc(sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->node);
    ptr->service_name = s_strdup(src->s_name);
    ptr->port = ntohs(src->s_port);

    return ptr;
}

void free_tcp_service_entry(tcp_service_entry *ptr) {
    free(ptr->service_name);
    ptr->service_name = NULL;
    free(ptr);
}

void free_tcp_service_entry_list(list_head *list) {
    struct tcp_service_entry *ptr;

    LIST_FOR_EACH_ENTRY(ptr, list, node) {
        list_del(&ptr->node);
        free_tcp_service_entry(ptr);
    }
}

void insert_tcp_service_entry(list_head *list, tcp_service_entry *src) {
    tcp_service_entry *ptr;

    LIST_FOR_EACH_ENTRY_REVERSE(ptr, list, node) {
        if (ptr->port <= src->port) {
            list_add(&src->node, &ptr->node);
            return;
        }
    }
    list_add_tail(&src->node, list);
}

void dns_resolve_main_4(int argc, char **argv) {
    char buf[MAXLINE];
    struct servent item, *result;
    DEFINE_LIST_HEAD(list);
    struct tcp_service_entry *ptr;
    int n, m;

    setservent(1);
    while(!getservent_r(&item, buf, MAXLINE, &result) && result == &item) {
        ptr = create_tcp_service_entry(result);
        if (ptr) {
            insert_tcp_service_entry(&list, ptr);
        }
    }
    endservent();

    n = 0;
    LIST_FOR_EACH_ENTRY(ptr, &list, node) {
        m = strlen(ptr->service_name);
        n = MAX(m, n);
    }

    n += 8;
    snprintf(buf, MAXLINE, "%%-%ds%%d\n", n);
    LIST_FOR_EACH_ENTRY(ptr, &list, node) {
        fprintf(stdout, buf, ptr->service_name, ptr->port);
    }

    free_tcp_service_entry_list(&list);
}

