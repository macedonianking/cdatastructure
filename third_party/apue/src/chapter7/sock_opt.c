#include "chapter7/sock_opt.h"

#define LOG_TAG "sock_opt"
#include "utils/net.h"
#include "utils/apue.h"
#include "utils/log.h"

typedef union sock_option_result {
    int length;
    int enable;
} sock_option_result;

typedef struct sock_option_t {
    int opt_level;
    int opt_option;
    const char *opt_name;
    void (*opt_describe_method)(struct sock_option_t*, sock_option_result *, socklen_t);
} sock_option_t;


static void check_socket_options(int fd);
static void print_sock_option_enable(struct sock_option_t *ptr, sock_option_result *result, socklen_t length);
static void print_sock_option_length(struct sock_option_t *ptr, sock_option_result *result, socklen_t length);

static const char *get_sock_level_name(int level) {
    switch(level) {
        case SOL_SOCKET: {
            return "SOL_SOCKET";
        }
        case IPPROTO_IP: {
            // IPV4
            return "IPPROTO_IP";
        }
        case IPPROTO_IPV6: {
            return "IPPROTO_IPV6";
        }
        case IPPROTO_TCP: {
            return "IPPROTO_TCP";
        }
        case IPPROTO_SCTP: {
            return "IPPROTO_SCTP";
        }
        default: {
            return NULL;
        }
    }
}


void chapter7_sock_opt_main(int argc, char **argv) {
    chapter7_sock_opt_main_3(argc, argv);
}

void chapter7_sock_opt_main_1(int argc, char **argv) {
    int fd;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    ALOGE_ALWAYSE_FATAL_IF(fd == -1, "create tcp socket FATAL(%s)", strerror(errno));
    check_socket_options(fd);
    close(fd);
}

// 输出socket的默认选项
void check_socket_options(int fd) {
    struct sock_option_t options[] = {
        { SOL_SOCKET, SO_DEBUG, "SO_DEBUG", &print_sock_option_enable },
        { SOL_SOCKET, SO_BROADCAST, "SO_BROADCAST", &print_sock_option_enable },
        { SOL_SOCKET, SO_REUSEADDR, "SO_REUSEADDR", &print_sock_option_enable },
        { SOL_SOCKET, SO_KEEPALIVE, "SO_KEEPALIVE", &print_sock_option_enable },
        { SOL_SOCKET, SO_SNDBUF, "SO_SNDBUF", &print_sock_option_length },
        { SOL_SOCKET, SO_RCVBUF, "SO_RCVBUF", &print_sock_option_length },
        { SOL_SOCKET, SO_SNDLOWAT, "SO_SNDLOWAT", &print_sock_option_length },
        { SOL_SOCKET, SO_RCVLOWAT, "SO_RCVLOWAT", &print_sock_option_length },
    };
    sock_option_t *ptr;
    sock_option_result result;
    socklen_t length;
    int n;

    n = NARRAY(options);
    for (int i = 0; i < n; ++i) {
        ptr = options + i;
        length = sizeof(result);
        if (!getsockopt(fd, ptr->opt_level, ptr->opt_option, &result, &length)) {
            if (ptr->opt_describe_method) {
                ptr->opt_describe_method(ptr, &result, length);
            }
        }
    }
}

// 输出enable和disable的选项
void print_sock_option_enable(struct sock_option_t *ptr, sock_option_result *result, socklen_t length) {
    if (length != sizeof(int)) {
        return;
    }
    fprintf(stdout, "%s:%s\t-->[%s]\n",
            get_sock_level_name(ptr->opt_level),
            ptr->opt_name,
            result->enable ? "enable" : "disable");
}

// 输出长度的选项
void print_sock_option_length(struct sock_option_t *ptr, sock_option_result *result, socklen_t length) {
    if (length != sizeof(int)) {
        return;
    }
    fprintf(stdout, "%s:%s\t-->[%d]\n",
            get_sock_level_name(ptr->opt_level),
            ptr->opt_name,
            result->length);
}

void chapter7_sock_opt_main_2(int argc, char **argv) {
    int fd;
    struct sockaddr_in addr;
    char buf[MAXLINE];
    int enable;

    if (resolve_host("www.baidu.com", "http", &addr)) {
        return;
    }
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return;
    }
    enable = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_DONTROUTE, &enable, sizeof(int))) {
        ALOGE("setsockopt failure(%s)", strerror(errno));
        goto out;
    }
    if (connect(fd, (SA*) &addr, sizeof(addr))) {
        ALOGE("connect %s failure(%s)", sock_ntop((SA*) &addr, sizeof(addr), buf, MAXLINE),
            strerror(errno));
        goto out;
    }
    ALOGD("connect to %s success", sock_ntop((SA*) &addr, sizeof(addr), buf, MAXLINE));
out:
    close(fd);
}

void chapter7_sock_opt_main_3(int argc, char **argv) {
    int fd;
    int eanble;
    socklen_t length;

    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return;
    }
    length = sizeof(eanble);
    eanble = 1;
    if (!getsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &eanble, &length)) {
        fprintf(stdout, "tcp_eanble=%d\n", eanble);
    }
    close(fd);
}

