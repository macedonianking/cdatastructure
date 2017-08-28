#include "chapter1/daytime_client.h"

#define LOG_TAG "daytime_client"
#include "utils/apue.h"
#include "utils/net.h"
#include "utils/log.h"
#include "utils/utils.h"

void daytime_client_main(int argc, char **argv) {
    daytime_client_main_3(argc, argv);
}

void daytime_client_main_1(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    char buf[MAXLINE];
    int n;

    bzero(&addr, sizeof(addr));
    if (resolve_host("s1a.time.edu.cn", "daytime", &addr) == -1) {
        return;
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(13);

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
    }

    if (connect(fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        ALOGE("connect failure:%s", strerror(errno));
        goto out;
    }

    while ((n = read(fd, buf, MAXLINE - 1)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

out:
    close(fd);
    return;
}

/**
 * 发送本地时间信息
 */
static void send_time_message(int fd) {
    char buf[MAXLINE], *src;
    int n, remain;
    time_t seconds;
    struct tm tm_obj;

    time(&seconds);
    localtime_r(&seconds, &tm_obj);
    n = strftime(buf, MAXLINE, "%a %b %d %T %Y\r\n", &tm_obj);

    src = buf;
    remain = n;
    while (remain > 0) {
        if ((n = write(fd, src, remain)) > 0) {
            remain -= n;
            src += n;
        } else {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
    }
}

/**
 * 服务端程序
 */
static void daytime_client_main_2_server(int argc, char **argv) {
    struct sockaddr_in addr, client_addr;
    socklen_t length;
    int fd, client_fd;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SERVER_PORT);
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
    }
    if (bind(fd, (SA*) &addr, sizeof(addr)) == -1) {
        goto out;
    }
    if (listen(fd, LISTEN_BACKLOG_COUNT) == -1) {
        goto out;
    }

    for (;;) {
        length = sizeof(client_addr);
        if ((client_fd= accept(fd, (SA*) &client_addr, &length)) == -1) {
            break;
        }
        send_time_message(client_fd);
        close(client_fd);
    }

out:
    close(fd);
    return;
}

static void daytime_client_main_2_client(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    char buf[MAXLINE];
    int n;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    if (get_loopback_addr(&addr.sin_addr)) {
        return;
    }

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
    }
    if (connect(fd, (SA*)&addr, sizeof(addr)) == -1) {
        goto out;
    }

    while ((n = read(fd, buf, MAXLINE)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

out:
    close(fd);
}

void daytime_client_main_2(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &daytime_client_main_2_server,
        &daytime_client_main_2_client);
}

void daytime_client_main_3(int argc, char **argv) {
    int fd;

    if ((fd = socket(9999, SOCK_STREAM, 0)) == -1) {
        ALOGE("socket failure:%s", strerror(errno));
    }
    if (fd != -1) {
        close(fd);
    }
}

void daytime_client_exercise_1_4(int fd) {
    char buf[MAXLINE];
    int n, counter;

    counter = 0;
    for (;;) {
        if((n = read(fd, buf, MAXLINE)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            break;
        }
        if (!n) {
            continue;
        }
        ++counter;
        write(STDOUT_FILENO, buf, n);
    }
    ALOGD("counter=%d", counter);
}
