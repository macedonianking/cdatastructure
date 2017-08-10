#include "chapter8/udp_server.h"

#define LOG_TAG "udp_server"
#include "utils/apue.h"
#include "utils/net.h"

void udp_server_main(int argc, char **argv) {
    udp_server_main_1(argc, argv);
}

static void udp_server_main_1_server(int argc, char **argv);
static void udp_server_main_1_client(int argc, char **argv);

void udp_server_main_1(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &udp_server_main_1_server,
        &udp_server_main_1_client);
}

void udp_server_main_1_server_dg_echo(int fd) {

}

void udp_server_main_1_server(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    int result;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ALOGE_ALWAYSE_FATAL_IF(fd == -1, "create udp socket FATAL:%s", strerror(errno));
    result = get_local_socket_addr(&addr, SERVER_PORT);
    ALOGE_ALWAYSE_FATAL_IF(result == -1, "get_local_socket_addr FATAL:%s", strerror(errno));
    result = bind(fd, (struct sockaddr*) &addr, sizeof(addr));
    ALOGE_ALWAYSE_FATAL_IF(result == -1, "bind FATAL:%s", strerror(errno));

    udp_server_main_1_server_dg_echo(fd);

    close(fd);
}

void udp_server_main_1_client(int argc, char **argv) {

}
