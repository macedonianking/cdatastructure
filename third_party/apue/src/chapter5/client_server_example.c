#include "chapter5/client_server_example.h"

#include <limits.h>

#define LOG_TAG "client_server_example"
#include "utils/net.h"
#include "utils/log.h"
#include "utils/apue.h"

/**
 * The server read from client and write to stdout and write back to client fd.
 */
static int server_echo_client(int fd);

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

static int server_echo_read(int fd, char *buf, int size, int *out_count) {
    struct pollfd wait;
    int count, remain, is_newline;
    int n;
    int r;

    count = 0;
    remain = size - 1;
    memset(&wait, 0, sizeof(wait));
    wait.fd = fd;
    wait.events = POLLIN | POLLPRI;

    r = 0;
    while (remain > 0) {
        if (poll(&wait, 1, -1) == -1) {
            if (errno == EINTR) {
                continue;
            }
            r = -1;
            break;
        } else if (wait.revents & (POLLIN | POLLPRI)) {
            if ((n = read(fd, buf + count, remain)) == -1) {
                if (errno == EINTR) {
                    continue;
                }
                r = -1;
                break;
            } else if (!n) {
                break;
            } else {
                buf[count + n] = '\n';
                is_newline = strchr(buf + count, '\n') != NULL;
                count += n;
                remain -= n;
                if (is_newline || !remain) {
                    break;
                }
            }
        } else if (wait.revents & (POLLERR | POLLHUP | POLLINVAL)) {
            r = -1;
            break;
        }
    }

    // There is data, make the call successful.
    if (r && count) {
        r = 0;
    }
    return r;
}

static int server_echo_write(int fd, char *buf, int count) {
    return 0;
}

int server_echo_client(int fd) {
    char buf[LINE_MAX];
    int count;
    int n;

    r = 0;
    apue_set_fl(fd, O_NONBLOCK);
    for (;;) {
        if (server_echo_read(fd, buf, LINE_MAX, &count)) {
            r = -1;
            break;
        }
        if (!count) {
            break;
        }
        if (server_echo_write(fd, buf, count)) {
            r = -1;
            break;
        }
    }

    return r;
}
