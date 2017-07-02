#include "chapter16/chapter16.h"

#include "utils/apue.h"
#include "utils/net.h"

#define BAIDU_IP    "14.215.177.38"

int chapter16_main(int argc, char **argv) {
    chapter16_3_2_4(argc, argv);
    return 0;
}

void chapter16_2(int argc, char **argv) {
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        LOGE("socket FATAL");
    }
    close(fd);
}

void chapter16_3(int argc, char **argv) {
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = 8080;
    if (inet_pton(AF_INET, BAIDU_IP, &addr.sin_addr) <= 0) {
        LOGE("inet_pton FATAL");
    }
}

/**
 * 查询本机配置
 */
void chapter16_3_2_1(int argc, char **argv) {
    char buf[INET6_ADDRSTRLEN];
    struct hostent *ent;
    char **aliases, **addr_list;

    sethostent(1);
    while ((ent = gethostent())) {
        fprintf(stdout, "name: %s\n", ent->h_name);
        aliases = ent->h_aliases;
        while (*aliases) {
            fprintf(stdout, "    aliase: %s\n", *aliases++);
        }
        addr_list = ent->h_addr_list;
        while (*addr_list) {
            if (ent->h_addrtype == AF_INET && inet_ntop(AF_INET, *addr_list, buf, INET6_ADDRSTRLEN)) {
                fprintf(stdout, "    AF_INET: %s\n", buf);
            } else if (ent->h_addrtype == AF_INET6 && inet_ntop(AF_INET6, *addr_list, buf, INET6_ADDRSTRLEN)) {
                fprintf(stdout, "    AF_INET6: %s\n", buf);
            }
            ++addr_list;
        }
    }
    endhostent();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

void chapter16_3_2_2(int argc, char **argv) {
    char buf[INET6_ADDRSTRLEN];
    struct netent *ent;

    setnetent(1);
    while ((ent = getnetent())) {
        fprintf(stdout, "name: %s\n", ent->n_name);
    }
    endnetent();
}
#pragma GCC diagnostic pop

void chapter16_3_2_3(int argc, char **argv) {
    struct protoent *ent;
    char **aliases;

    setprotoent(1);
    while ((ent = getprotoent())) {
        fprintf(stdout, "name: %s\n", ent->p_name);
        fprintf(stdout, "number: %d\n", ent->p_proto);
        aliases = ent->p_aliases;
        while (*aliases) {
            fprintf(stdout, "    aliases: %s\n", *aliases++);
        }
    }
    endprotoent();

    fprintf(stdout, "end\n");
    if ((ent = getprotobyname("tcp"))) {
        fprintf(stdout, "name: %s number=%d\n",
            ent->p_name, ent->p_proto);
    }
}

void chapter16_3_2_4(int argc, char **argv) {
    struct servent *ent;

    setservent(1);
    while ((ent = getservent())) {
        fprintf(stdout, "name: %s\n", ent->s_name);
        fprintf(stdout, "port: %d\n", ent->s_port);
    }
    endservent();
}
