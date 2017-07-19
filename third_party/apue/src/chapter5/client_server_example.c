#include "chapter5/client_server_example.h"

#define LOG_TAG "client_server_example"
#include "utils/net.h"
#include "utils/log.h"
#include "utils/apue.h"

void client_server_example_main(int argc, char **argv) {
    client_server_example_main_1(argc, argv);
}

static void client_server_example_main_1_client(int argc, char **argv) {

}

static void server_process_service_client(int fd, int client_fd, struct sockaddr_in *addr, socklen_t len) {

}

/**
 * The server application main routing.
 */
static void client_server_example_main_1_server(int argc, char **argv) {
    struct sockaddr_in addr, client_addr;
    int fd, client_fd;
    socklen_t addr_len;

    if (get_local_socket_addr(&addr, SERVER_PORT)) {
        goto meet_error;
    }
    if ((fd = create_server_socket(&addr, LISTEN_BACKLOG_COUNT)) == -1) {
        goto meet_error;
    }

    for (;;) {
        addr_len = sizeof(struct sockaddr_in);
        if ((client_fd = accept(fd, (struct sockaddr*) &client_addr, &addr_len)) == -1) {
            if (errno == EINTR) {
                continue;
            } else if (errno == EINVAL) {
                goto meet_error;
            } else {
                ALOGE("accept failure(%s), will continue", strerror(errno));
                continue;
            }
        }
        server_process_service_client(fd, client_fd, &addr, addr_len);
    }

meet_error:
    ALOGE("client_server_example_main_1_server FATAL");
    exit(-1);
}

void client_server_example_main_1(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &client_server_example_main_1_server,
        &client_server_example_main_1_client);
}


