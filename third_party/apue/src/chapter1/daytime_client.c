#include "chapter1/daytime_client.h"

#define LOG_TAG "daytime_client"
#include "utils/apue.h"
#include "utils/net.h"
#include "utils/log.h"

#define SERVER_OPTION   "--server"
#define CLIENT_OPTION   "--client"

void daytime_client_main(int argc, char **argv) {
    daytime_client_main_2(argc, argv);
}

void daytime_client_main_1(int argc, char **argv) {
    struct sockaddr_in addr;
    char ip_addr[INET6_ADDRSTRLEN];

    if (resolve_host("www.baidu.com", "http", &addr)) {
        return;
    }
    if (inet_ntop(AF_INET, &addr.sin_addr, ip_addr, INET6_ADDRSTRLEN) == ip_addr) {
        fprintf(stdout, "ip=%s, port=%d\n", ip_addr, ntohs(addr.sin_port));
    }
}

static void send_time_message(int fd) {
    time_t time_in_seconds;

    time(&time_in_seconds);
    dprintf(fd, "%s", ctime(&time_in_seconds));
}

static void daytime_client_main_2_server(int argc, char **argv) {
    struct sockaddr_in addr;
    int r;
    int fd, conn_fd;

    r = get_interface_addr(&addr.sin_addr);
    LOG_ALWAYSE_FATAL_IF(r, "get_interface_addr FATAL");
    addr.sin_family = AF_INET;
    addr.sin_port = SERVER_PORT;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    LOG_ALWAYSE_FATAL_IF(fd == -1, "daytime_client_main_2_server. create socket FATAL(%s)",
        strerror(errno));

    r = bind(fd, (struct sockaddr*)&addr, sizeof(addr));
    LOG_ALWAYSE_FATAL_IF(r, "daytime_client_main_2_server. bind FATAL(%s)",
        strerror(errno));

    r = listen(fd, 10);
    LOG_ALWAYSE_FATAL_IF(r, "daytime_client_main_2_server. listen FATAL(%s)",
        strerror(errno));

    for (;;) {
        conn_fd = accept(fd, NULL, NULL);
        if (conn_fd == -1) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("daytime_client_main_2_server. accept FATAL(%s)", strerror(errno));
            exit(-1);
        }
        send_time_message(conn_fd);
        close(conn_fd);
    }
}

static void daytime_client_main_2_client(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
    }
    if (get_interface_addr(&addr.sin_addr)) {
        goto out;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = SERVER_PORT;
    for (;;) {
        if (connect(fd, (struct sockaddr*) &addr, sizeof(addr))) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("daytime_client_main_2_client FATAL(%s)", strerror(errno));
            goto out;
        }
        break;
    }
    apue_fd_copy(fd, STDOUT_FILENO);
out:
    close(fd);
}

void daytime_client_main_2(int argc, char **argv) {
    if (argc < 2) {
        ALOGE("daytime_client_main_2 argument is not enough");
        return;
    }

    if (!strcmp(SERVER_OPTION, argv[1])) {
        daytime_client_main_2_server(argc, argv);
    } else if (!strcmp(CLIENT_OPTION, argv[1])) {
        daytime_client_main_2_client(argc, argv);
    } else {
        ALOGE("daytime_client_main_2 unknown option %s", argv[1]);
    }
}
