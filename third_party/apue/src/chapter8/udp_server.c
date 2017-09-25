#include "chapter8/udp_server.h"

#include <limits.h>

#define LOG_TAG "udp_server"
#include "utils/apue.h"
#include "utils/net.h"

void udp_server_main(int argc, char **argv) {
    udp_server_main_3(argc, argv);
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

static int upd_writen(int fd, struct sockaddr_in *addr, char *buf, int size) {
    int n;
    int r;

    r = -1;
    for (;;) {
        if ((n = sendto(fd, buf, size, 0, (SA*)addr, sizeof(*addr))) == -1) {
            if (errno == EINTR) {
                continue;
            }
            r = -1;
            break;
        }
        r = n == size ? 0 : -1;
        break;
    }
    return r;
}

static void upd_server_echo(int fd) {
    char buf[MAXLINE];
    int n;
    struct sockaddr_in addr;
    socklen_t length;

    for (;;) {
        length = sizeof(addr);
        if ((n = recvfrom(fd, buf, MAXLINE, 0, (SA*) &addr, &length)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }

        if (upd_writen(fd, &addr, buf, n)) {
            break;
        }
    }
}

void udp_server_main_1_server(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1 || bind(fd, (SA*) &addr, sizeof(addr))) {
        if (fd != -1) {
            close(fd);
        }
        return;
    }
    upd_server_echo(fd);
    close(fd);
}


static void upd_client_echo(int fd) {
    char buf[MAXLINE];
    struct sockaddr_in addr;
    int n;

    get_local_socket_addr(&addr, SERVER_PORT);
    for (;;) {
        if ((n = read(STDIN_FILENO, buf, MAXLINE)) == -1 || !n) {
            break;
        }
        if ((n = sendto(fd, buf, n, 0, (SA*) &addr, sizeof(addr))) == -1 || !n) {
            break;
        }
        if ((n = recvfrom(fd, buf, MAXLINE, 0, NULL, NULL)) == -1) {
            break;
        }
        if ((n = write(fd, buf, n)) == -1) {
            break;
        }
    }
}

/**
 * 客户端的主代码
 */
void udp_server_main_1_client(int argc, char **argv) {
    int fd;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return;
    }
    upd_client_echo(fd);
    close(fd);
}


/**
 * connected udp socket operation.
 */
void udp_server_main_3(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    socklen_t length;
    char buf[MAXLINE];
    int n, m;

    get_local_socket_addr(&addr, SERVER_PORT);
    length = sizeof(addr);
    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        return;
    }
    if (connect(fd, (SA*) &addr, length)) {
        close(fd);
        return;
    }

    for (;;) {
        if ((n = read(STDIN_FILENO, buf, MAXLINE)) == -1 || !n) {
            break;
        }
        if ((n = sendto(fd, buf, n, 0, (SA*) &addr, length)) == -1 || !n) {
            break;
        }
        if ((n = read(fd, buf, MAXLINE)) == -1) {
            ALOGE("read FATAL(%s)", strerror(errno));
            break;
        }
        if ((m = write(fd, buf, n)) == -1 || m != n) {
            break;
        }
    }
    close(fd);
}
