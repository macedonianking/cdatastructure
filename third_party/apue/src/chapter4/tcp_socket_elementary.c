#include "chapter4/tcp_socket_elementary.h"

#define LOG_TAG "tcp_socket_elementary"
#include "utils/apue.h"
#include "utils/net.h"

void chapter4_tcp_socket_elementary(int argc, char **argv) {
    chapter4_tcp_socket_exercise_2(argc, argv);
}

/**
 * socket create server and client file descriptor.
 */
void chapter4_tcp_socket_elementary_1(int argc, char **argv) {
    int client_fd;
    struct sockaddr_in addr;

    if ((client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        ALOGE("chapter4_tcp_socket_elementary_1 create tcp socket FATAL(%s)", strerror(errno));
        exit(-1);
    }
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(0);
    if (bind(client_fd, (struct sockaddr*) &addr, sizeof(addr))) {
        ALOGE("bind failure(%s)", strerror(errno));
    }

    close(client_fd);
}

static void log_one_service(struct sockaddr_in *addr) {
    ALOGE("log_one_service-->%s", sock_ntop((struct sockaddr*) addr, sizeof(*addr)));
}

static void chapter4_tcp_socket_elementary_2_server(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    socklen_t len;
    int client_fd;

    if (inet_pton(AF_INET, LOCAL_IP_ADDR, &addr.sin_addr) != 1) {
        ALOGE("chapter4_tcp_socket_elementary_2_server inet_pton failure");
        return;
    }
    addr.sin_family= AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        ALOGE("chapter4_tcp_socket_elementary_2_server socket failure(%s)", strerror(errno));
        return;
    }
    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr))) {
        ALOGE("chapter4_tcp_socket_elementary_2_server bind failure(%s)", strerror(errno));
        goto out;
    }
    if (listen(fd, LISTEN_BACKLOG_COUNT)) {
        ALOGE("chapter4_tcp_socket_elementary_2_server listen failure(%s)", strerror(errno));
        goto out;
    }

    for (;;) {
        if ((client_fd = accept(fd, (struct sockaddr*)&addr, &len)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("chapter4_tcp_socket_elementary_2_server accept failure(%s)", strerror(errno));
            continue;
        }
        say_daytime(client_fd);
        log_one_service(&addr);
        close(client_fd);
    }

out:
    close(fd);
}

static void chapter4_tcp_socket_elementary_2_client(int argc, char **argv) {
    int fd;

    if ((fd = create_local_connection(SERVER_PORT)) == -1) {
        return;
    }
    apue_fd_copy(fd, STDOUT_FILENO);
    close(fd);
}

void chapter4_tcp_socket_elementary_2(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &chapter4_tcp_socket_elementary_2_server,
        &chapter4_tcp_socket_elementary_2_client);
}

void chapter4_tcp_socket_exercise_2(int argc, char **argv) {
    struct sockaddr_in addr, out_addr;
    socklen_t out_addr_len;
    int fd;

    if (resolve_host("www.baidu.com", "http", &addr)) {
        ALOGE("resolve_host failure");
        return;
    }
    if ((fd = create_connection(&addr)) == -1) {
        ALOGE("create_connection FATAL");
        return;
    }
    out_addr_len = sizeof(struct sockaddr_in);
    if (!getsockname(fd, (struct sockaddr*) &out_addr, &out_addr_len)) {
        ALOGD("getsockname:%s", sock_ntop((struct sockaddr*) &out_addr, out_addr_len));
    } else {
        ALOGE("getsockname failure(%s)", strerror(errno));
    }
    out_addr_len = sizeof(struct sockaddr_in);
    if (!getpeername(fd, (struct sockaddr*) &out_addr, &out_addr_len)) {
        ALOGD("getpeername:%s", sock_ntop((struct sockaddr*) &out_addr, out_addr_len));
    }
    close(fd);
}
