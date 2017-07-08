#include "chapter16/sock_addr.h"

#include <pthread.h>

#include "utils/apue.h"
#include "utils/net.h"

#define PC_HOST_IP      "172.17.151.69"

static void print_hostent(int fd, struct hostent *ent);
static void print_hostent_aliases(int fd, struct hostent *ent);
static void print_hostent_addr_list(int fd, struct hostent *ent);

static void print_name_list(int fd, char **list);
static void print_network_type(int fd, int type);

static int resolve_host(const char *node,
                        const char *service,
                        struct sockaddr_in *addr);

void chapter16_sock_addr_main(int argc, char **argv) {
    chapter16_sock_addr_10(argc, argv);
}

void chapter16_sock_addr_1(int argc, char **argv) {
    struct hostent *ent;

    ent = gethostbyname("baidu.com");
    if (ent) {
        print_hostent(STDOUT_FILENO, ent);
    }
}

void chapter16_sock_addr_2(int argc, char **argv) {
    struct hostent *ent;
    struct in_addr addr;

    if (inet_pton(AF_INET, PC_HOST_IP, &addr) == 1) {
        ent = gethostbyaddr(&addr, sizeof(addr), AF_INET);
        if (ent != NULL) {
            print_hostent(STDOUT_FILENO, ent);
        }
    }
}

void print_hostent(int fd, struct hostent *ent) {
    dprintf(fd, "Host:\n");
    dprintf(fd, "    name: %s\n", ent->h_name);
    print_hostent_aliases(fd, ent);

    dprintf(fd, "    addr type: ");
    print_network_type(fd, ent->h_addrtype);
    dprintf(fd, "\n");

    dprintf(fd, "    addr len: %u\n", ent->h_length);
    print_hostent_addr_list(fd, ent);
}

void print_hostent_aliases(int fd, struct hostent *ent) {
    dprintf(fd, "    aliases:");
    print_name_list(fd, ent->h_aliases);
    dprintf(fd, "\n");
}

void print_name_list(int fd, char **addr) {
    if (addr) {
        while (*addr) {
            dprintf(fd, " %s", *addr++);
        }
    }
}

void print_network_type(int fd, int type) {
    if (type == AF_INET) {
        dprintf(fd, "AF_INET");
    } else if (type == AF_INET6) {
        dprintf(fd, "AF_INET6");
    } else {
        dprintf(fd, "%d", type);
    }
}

void print_hostent_addr_list(int fd, struct hostent *ent) {
    char buf[INET6_ADDRSTRLEN];
    char **addr;

    dprintf(fd, "    addr list:");
    addr = ent->h_addr_list;
    if (addr) {
        while (*addr) {
            if (inet_ntop(ent->h_addrtype, *addr, buf, INET6_ADDRSTRLEN)) {
                dprintf(fd, " %s", buf);
            }
            ++addr;
        }
    }
    dprintf(fd, "\n");
}

/**
 * 输出网络配置
 */
static void print_netent(int fd, struct netent *ent);

void chapter16_sock_addr_3(int argc, char **argv) {
    struct netent *ent;

    setnetent(1);
    while ((ent = getnetent())) {
        print_netent(STDOUT_FILENO, ent);
    }
    endnetent();
}

void print_netent(int fd, struct netent *ent) {
    char buf[INET6_ADDRSTRLEN];
    int network_addr;

    dprintf(fd, "Network:\n");
    dprintf(fd, "    name: %s\n", ent->n_name);

    dprintf(fd, "    aliases:");
    print_name_list(fd, ent->n_aliases);
    dprintf(fd, "\n");

    dprintf(fd, "    addr type: ");
    print_network_type(fd, ent->n_addrtype);
    dprintf(fd, "\n");

    dprintf(fd, "    net: ");
    network_addr = htonl(ent->n_net);
    if (!inet_ntop(ent->n_addrtype, &network_addr, buf, INET6_ADDRSTRLEN)) {
        buf[0] = '\0';
    }
    dprintf(fd, "%s\n", buf);
}

static void print_protoent(int fd, struct protoent *ent) {
    dprintf(fd, "Protocol:\n");
    dprintf(fd, "    name: %s\n", ent->p_name);
    dprintf(fd, "    aliases:");
    print_name_list(fd, ent->p_aliases);
    dprintf(fd, "\n");
    dprintf(fd, "    proto: %d\n", ent->p_proto);
}

/**
 * 支持的所有协议和协议号
 */
void chapter16_sock_addr_4(int argc, char **argv) {
    struct protoent *ent;

    setprotoent(1);
    while ((ent = getprotoent())) {
        print_protoent(STDOUT_FILENO, ent);
    }
    endprotoent();
}

static void print_servent(int fd, struct servent *ent) {
    dprintf(fd, "Service:\n");
    dprintf(fd, "    name: %s\n", ent->s_name);
    dprintf(fd, "    aliases:");
    print_name_list(fd, ent->s_aliases);
    dprintf(fd, "\n");
    dprintf(fd, "    port: %d\n", ent->s_port);
    dprintf(fd, "    protocol: %s\n",ent->s_proto);
}

/**
 * 服务和端口号
 */
void chapter16_sock_addr_5(int argc, char **argv) {
    struct servent *ent;

    setservent(1);
    while ((ent = getservent())) {
        print_servent(STDOUT_FILENO, ent);
    }
    endservent();
}

void chapter16_sock_addr_6(int argc, char **argv) {
    struct protoent *ent;

    ent = getprotobyname("tcp");
    if (ent != NULL) {
        print_protoent(STDOUT_FILENO, ent);
    }
    ent = getprotobyname("udp");
    if (ent) {
        print_protoent(STDOUT_FILENO, ent);
    }
}

static void *chapter16_sock_addr_7_thread_func(void *args) {
    char buffer[BUFSIZ];
    struct servent serv_obj, *ret;

    if (!getservbyname_r("http", "tcp", &serv_obj, buffer, BUFSIZ, &ret)
        && ret == &serv_obj) {
        fprintf(stdout, "name=%s, port=%d, proto=%s\n",
            ret->s_name, ret->s_port, ret->s_proto);
    }
    if (!getservbyname_r("https", "tcp", &serv_obj, buffer, BUFSIZ, &ret)
        && ret == &serv_obj) {
        fprintf(stdout, "name=%s, port=%d, proto=%s\n",
            ret->s_name, ret->s_port, ret->s_proto);
    }
    return 0;
}

/**
 * 查询一台服务器有那些服务
 */
void chapter16_sock_addr_7(int argc, char **argv) {
    pthread_t tid;

    chapter16_sock_addr_7_thread_func(NULL);
    if ((pthread_create(&tid, NULL, &chapter16_sock_addr_7_thread_func, NULL))) {
        return;
    }
    pthread_join(tid, NULL);
}

static int look_up_protocol(const char *name, int *protocol) {
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

/**
 * AI_NUMERICALHOST
 */
void chapter16_sock_addr_8(int argc, char **argv) {
    char buffer[INET6_ADDRSTRLEN];
    struct addrinfo hint, *ret, *ptr;
    struct sockaddr_in *in_addr;
    int protocol;
    int r;

    if (look_up_protocol("tcp", &protocol)) {
        return;
    }

    memset(&hint, 0, sizeof(hint));
    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = protocol;

    if ((r = getaddrinfo("sina.com", "http", &hint, &ret))) {
        LOGE("getaddrinfo FATAL: %s", gai_strerror(r));
        return;
    }
    for (ptr = ret; ptr != NULL; ptr = ptr->ai_next) {
        if (ptr->ai_family == AF_INET && ptr->ai_addrlen == sizeof(*in_addr)) {
            in_addr = (struct sockaddr_in*) ptr->ai_addr;
            if (inet_ntop(in_addr->sin_family, &in_addr->sin_addr, buffer, INET6_ADDRSTRLEN) == buffer) {
                fprintf(stdout, "%s %s\n", ptr->ai_canonname, buffer);
            }
        }
    }
    freeaddrinfo(ret);
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

/**
 * 链接到www.baidu.com
 * connection. create connection.
 */
void chapter16_sock_addr_9(int argc, char **argv) {
    struct sockaddr_in addr;
    int fd;

    if (resolve_host("www.baidu.com", "http", &addr)) {
        LOGE("resolve_host FATAL");
        return;
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        LOGE("socket FATAL");
        return;
    }
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr))) {
        LOGE("connect FATAL");
    } else {
        LOGE("connect SUCCESS");
    }

    close(fd);
}

void chapter16_sock_addr_10(int argc, char **argv) {
}
