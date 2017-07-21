#include "chapter5/client_server_example.h"

#include <limits.h>

#define LOG_TAG "client_server_example"
#include "utils/net.h"
#include "utils/log.h"
#include "utils/apue.h"

#define STREAM_ECHO_FLUSH_WAY_IMMEDIATELY   0
#define STREAM_ECHO_FLUSH_WAY_NEWLINE       1

struct stream_echo_buf {
    char *buf;
    int size;
    int count;
    int remain;
    int flush_way;
    int require_flush;
};

/**
 * The server read from client and write to stdout and write back to client fd.
 */
static int server_echo_client(int fd);
/**
 * Client read from standard input and sokcet input, write to socket output and standard output.
 * and flush in different way.
 */
static int client_echo_server(int fd);

void client_server_example_main(int argc, char **argv) {
    client_server_example_main_1(argc, argv);
}

/**
 * 客户端主入口
 */
static void client_server_example_main_1_client(int argc, char **argv) {
    int fd;

    if ((fd = create_local_connection(SERVER_PORT)) == -1) {
        ALOGE("create local connection %d FATAL", SERVER_PORT);
        exit(-1);
    }
    client_echo_server(fd);
    close(fd);
}

/**
 * 服务端处理客户端的请求
 */
static void server_process_service_client(int fd, int client_fd, struct sockaddr_in *addr, socklen_t len) {
    server_echo_client(client_fd);
    close(client_fd);
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

/**
 * 程序主入口
 */
void client_server_example_main_1(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &client_server_example_main_1_server,
        &client_server_example_main_1_client);
}

/**
 * 服务端读取数据到buf, 遇到newline或者bug区满，哪儿返回
 */
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
        } else if (wait.revents & (POLLERR | POLLHUP | POLLNVAL)) {
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

/**
 * 服务端响应客户端过程
 */
int server_echo_client(int fd) {
    char buf[LINE_MAX];
    int count;
    int r;

    r = 0;
    apue_set_fl(fd, O_NONBLOCK);
    for (;;) {
        // 读取数据
        if (server_echo_read(fd, buf, LINE_MAX, &count)) {
            r = -1;
        } else if (!count) {
            break;
        } else if (writen(fd, buf, count) != count) {
            // 写入数据出错
            r = -1;
            break;
        }
    }
    return r;
}

static void init_wait_object(struct pollfd *item, int fd) {
    memset(item, 0, sizeof(struct pollfd));
    item->fd = fd;
    item->events = POLLIN | POLLPRI;
    item->revents = 0;
}

/**
 * 初始化echo的缓冲区
 */
static void init_stream_echo_buf(struct stream_echo_buf *ptr) {
    memset(ptr, 0, sizeof(*ptr));
    ptr->size = LINE_MAX;
    ptr->buf = (char*) malloc(sizeof(char) * ptr->size);
    ALOGE_ALWAYSE_FATAL_IF(!ptr->buf, "malloc %ld bytes failure(%s)", sizeof(char) * ptr->size, strerror(errno));
    ptr->count = 0;
    ptr->remain = ptr->size - 1;
    ptr->flush_way = STREAM_ECHO_FLUSH_WAY_IMMEDIATELY;
    ptr->require_flush = 0;
}

static void free_stream_echo_buf(struct stream_echo_buf *ptr) {
    free(ptr->buf);
    memset(ptr, 0, sizeof(*ptr));
}

static int client_echo_rd_data(struct pollfd *waits, struct stream_echo_buf *sock_buf,
        struct stream_echo_buf *user_buf) {
    int r;
    struct pollfd *user_wait, *sock_wait;

    user_wait = waits;
    sock_wait = waits + 1;
    r = 0;
    for (;;) {
        if (user_wait->fd == -1 && sock_wait->fd == -1) {
            r = -1;
            break;
        } else if(user_wait->fd == -1) {

        } else if(sock_wait->fd == -1) {

        } else {

        }
    }

    return r;
}

/**
 * 客户端的代码写入数据
 */
static int client_echo_wr_data(int fd, struct stream_echo_buf *sock_buf, struct stream_echo_buf *user_buf) {
    int r;

    r = 0;
    if (sock_buf->require_flush) {
        if (writen(STDOUT_FILENO, sock_buf->buf, sock_buf->count) != sock_buf->count) {
            r = -1;
        }
        sock_buf->count = 0;
        sock_buf->require_flush = 0;
    }
    if (user_buf->require_flush) {
        if (writen(fd, user_buf->buf, user_buf->count) != user_buf->count) {
            r = -1;
        }
        user_buf->count = 0;
        user_buf->require_flush = 0;
    }

    return r;
}

/**
 * 客户端响应服务端和标注输入
 */
int client_echo_server(int fd) {
    int r;
    struct pollfd waits[2];
    struct stream_echo_buf user_buf, sock_buf;

    // 初始化对象
    init_wait_object(waits, STDIN_FILENO);
    init_wait_object(waits + 1, STDOUT_FILENO);
    init_stream_echo_buf(&user_buf);
    init_stream_echo_buf(&sock_buf);
    user_buf.flush_way = STREAM_ECHO_FLUSH_WAY_NEWLINE;
    sock_buf.flush_way = STREAM_ECHO_FLUSH_WAY_IMMEDIATELY;
    r = 0;

    for (;;) {
        if (waits[0].fd == -1 && waits[1].fd == -1) {
            break;
        }
        if (client_echo_rd_data(waits, &sock_buf, &user_buf)) {
            // Read data from data.
            r = -1;
        }
        if (sock_buf.require_flush || user_buf.require_flush) {
            if (client_echo_wr_data(fd, &sock_buf, &user_buf)) {
                // If write data meet an error, stop immediately.
                r = -1;
                break;
            }
        }
    }

    // 销毁对象内部状态
    free_stream_echo_buf(&user_buf);
    free_stream_echo_buf(&sock_buf);
    close(fd);
    return r;
}