#include "chapter16/sock_addr.h"

#include "utils/apue.h"
#include "utils/net.h"

#define PC_HOST_IP      "172.17.151.69"

static void print_hostent(int fd, struct hostent *ent);
static void print_hostent_aliases(int fd, struct hostent *ent);
static void print_hostent_addr_list(int fd, struct hostent *ent);

static void print_name_list(int fd, char **list);
static void print_network_type(int fd, int type);

void chapter16_sock_addr_main(int argc, char **argv) {
    chapter16_sock_addr_3(argc, argv);
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
