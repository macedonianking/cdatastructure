#include "chapter8/udp_server.h"

#include <limits.h>

#define LOG_TAG "udp_server"
#include "utils/apue.h"
#include "utils/net.h"

void udp_server_main(int argc, char **argv) {
    udp_server_main_2(argc, argv);
}

/**
 * UDP协议服务端入口
 */
static void udp_server_main_1_server(int argc, char **argv);
/**
 * UDP协议客户端入口
 */
static void udp_server_main_1_client(int argc, char **argv);

void udp_server_main_1(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &udp_server_main_1_server,
        &udp_server_main_1_client);
}

/**
 * server echo client datagram.
 */
static void udp_server_main_1_server_main_routing(int fd) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;
    char buf[LINE_MAX];
    int n, write_count;

    for (;;) {
        client_addr_len = sizeof(client_addr);
        if ((n = recvfrom(fd, buf, LINE_MAX - 1, 0, (SA*)&client_addr, &client_addr_len)) == -1) {
            ALOGE("recvfrom error: %s", strerror(errno));
            break;
        }
        if (n != 0) {
            if ((write_count = sendto(fd, buf, n, 0, (SA*)&client_addr, client_addr_len)) == -1) {
                ALOGE("sendto error: %s", strerror(errno));
            } else if (write_count != n) {
                ALOGE("sendto write part data: n=%d, write_count=%d", n, write_count);
            }
        }
    }
}

void udp_server_main_1_server(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    int result;

    /**
     * 创建UDP的socket后绑定本地的地址
     */
    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ALOGE_ALWAYSE_FATAL_IF(fd == -1, "create udp socket FATAL:%s", strerror(errno));
    result = get_local_socket_addr(&addr, SERVER_PORT);
    ALOGE_ALWAYSE_FATAL_IF(result == -1, "get_local_socket_addr FATAL:%s", strerror(errno));
    result = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
    ALOGE_ALWAYSE_FATAL_IF(result == -1, "bind FATAL:%s", strerror(errno));

    /**
     * 创建UDP的socket之后使用socket
     */
    udp_server_main_1_server_main_routing(fd);

    /**
     * Should not be here.
     */
    close(fd);
}

/**
 * 客户端主程序入口
 */
static void udp_server_main_1_client_main_routing(int argc, char **argv, int fd,
    struct sockaddr_in *server_addr) {
    struct sockaddr_in addr;
    char buf[LINE_MAX];
    int n, count;
    socklen_t length;

    for (;;) {
        if ((n = read(STDIN_FILENO, buf, LINE_MAX - 1)) <= 0) {
            break;
        }

        if ((count = sendto(fd, buf, n, 0, (SA*)server_addr, sizeof(*server_addr))) == -1) {
            ALOGE("sendto FATAL: %s", strerror(errno));
            break;
        }
        else if (count != n) {
            ALOGE("sendto: n = %d, count = %d", n, count);
        }

        length = sizeof(addr);
        if ((n = recvfrom(fd, buf, LINE_MAX - 1, 0, (SA*) &addr, &length)) == -1) {
            ALOGE("recvfrom FATAL:%s", strerror(errno));
            break;
        }
        write(STDOUT_FILENO, buf, n);
    }
}

/**
 * 客户端的主代码
 */
void udp_server_main_1_client(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    int result;

    /**
     * 创建socket和获取服务端的地址
     */
    result = get_local_socket_addr(&addr, SERVER_PORT);
    ALOGE_ALWAYSE_FATAL_IF(result == -1, "get_local_socket_addr FATAL:%s", strerror(errno));
    fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    ALOGE_ALWAYSE_FATAL_IF(fd == -1, "client create udp socket FATAL:%s", strerror(errno));

    /**
     * 主程序入口
     */
    udp_server_main_1_client_main_routing(argc, argv, fd, &addr);
}

static void udp_server_main_2_client_datagram_echo(int fd, struct sockaddr_in *addr) {
    char buf[LINE_MAX];
    int r, n, count;
    socklen_t length;

    length = sizeof(*addr);
    if ((r = connect(fd, (SA*) addr, length)) == -1) {
        ALOGE("connect FATAL:%s", strerror(errno));
        return;
    }

    for (;;) {
        if ((n = read(STDIN_FILENO, buf, LINE_MAX - 1)) == -1 || n == 0) {
            break;
        }
        if ((count = write(fd, buf, n)) == -1) {
            ALOGE("udp socket write FATAL:%s", strerror(errno));
            break;
        }
        if (count != n) {
            break;
        }
        if ((count = read(fd, buf, LINE_MAX - 1)) == -1) {
            ALOGE("upd socket read FATAL:%s", strerror(errno));
            break;
        }
        write(STDOUT_FILENO, buf, count);
    }
}

static void udp_server_main_2_client(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return;
    }
    if ((get_local_socket_addr(&addr, SERVER_PORT)) == -1) {
        goto out;
    }
    udp_server_main_2_client_datagram_echo(fd, &addr);

out:
    close(fd);
    return;
}

void udp_server_main_2(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &udp_server_main_1_server,
        &udp_server_main_2_client);
}
