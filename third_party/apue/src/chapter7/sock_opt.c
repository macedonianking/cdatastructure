#include "chapter7/sock_opt.h"

#define LOG_TAG "sock_opt"
#include "utils/net.h"
#include "utils/apue.h"
#include "utils/log.h"

typedef union _sock_opt {
    int enabled;
    int length;
    long long_value;
    struct linger linger_value;
    struct timeval time;
} sock_opt;

struct socket_opt_item {
    int socket_level;
    int socket_option;
    const char *name;
    const char *(*opt_str_method)(struct socket_opt_item *item, sock_opt *opt);
};


static char _socket_opt_buf[BUFSIZ];
static struct linger _socket_linger;

extern const char *opt_str_enabled(struct socket_opt_item *item, sock_opt *opt);
extern const char *opt_str_length(struct socket_opt_item *item, sock_opt *opt);
extern const char *opt_str_long(struct socket_opt_item *item, sock_opt *opt);
extern const char *opt_str_linger(struct socket_opt_item *item, sock_opt *opt);
extern const char *opt_str_time(struct socket_opt_item *item, sock_opt *opt);

/**
 * 初始化对象
 */
static void initialize_sock_option() {
    _socket_linger.l_onoff = 1;
    _socket_linger.l_linger = 10;
}

void chapter7_sock_opt_main(int argc, char **argv) {
    initialize_sock_option();
    chapter7_sock_opt_main_2(argc, argv);
}

void chapter7_sock_opt_main_1(int argc, char **argv) {
    int fd;
    sock_opt opt;
    socklen_t length;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ALOGE_ALWAYSE_FATAL_IF(fd == -1, "create socket fatal(%s)", strerror(errno));

    opt.enabled = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_DEBUG, &opt, sizeof(opt))) {
        ALOGE("setsockopt fatal(%s)", strerror(errno));
        exit(-1);
    }
    length = sizeof(opt);
    if (getsockopt(fd, SOL_SOCKET, SO_DEBUG, &opt, &length)) {
        ALOGE("getsockopt fatal(%s)", strerror(errno));
        exit(-1);
    } else {
        ALOGE("getsockopt value is %d", opt.enabled);
    }

    close(fd);
}

const char *opt_str_enabled(struct socket_opt_item *item, sock_opt *opt) {
    snprintf(_socket_opt_buf, NARRAY(_socket_opt_buf), "%s is %s",
        item->name, opt->enabled ? "enabled" : "disabled");
    return _socket_opt_buf;
}

const char *opt_str_length(struct socket_opt_item *item, sock_opt *opt) {
    snprintf(_socket_opt_buf, NARRAY(_socket_opt_buf), "%s length is %d",
        item->name, opt->length);
    return _socket_opt_buf;
}

const char *opt_str_long(struct socket_opt_item *item, sock_opt *opt) {
    snprintf(_socket_opt_buf, NARRAY(_socket_opt_buf), "%s is %ld",
        item->name, opt->long_value);
    return _socket_opt_buf;
}

const char *opt_str_linger(struct socket_opt_item *item, sock_opt *opt) {
    snprintf(_socket_opt_buf, NARRAY(_socket_opt_buf), "%s l_onoff is %d, l_linger is %d seconds",
        item->name, opt->linger_value.l_onoff, opt->linger_value.l_linger);
    return _socket_opt_buf;
}

const char *opt_str_time(struct socket_opt_item *item, sock_opt *opt) {
    snprintf(_socket_opt_buf, NARRAY(_socket_opt_buf), "%s %lds %ldus",
        item->name, opt->time.tv_sec, opt->time.tv_usec);
    return _socket_opt_buf;
}

static int prepare_socket_level_options(struct socket_opt_item *item, int n) {
    int m;
    m = 0;

    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_SNDBUF, "SO_SNDBUF", opt_str_length};
    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_RCVBUF, "SO_RCVBUF", opt_str_length};
    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_SNDLOWAT, "SO_SNDLOWAT", opt_str_length};
    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_RCVLOWAT, "SO_RCVLOWAT", opt_str_length};
    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_LINGER, "SO_LINGER", opt_str_linger};
    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_SNDTIMEO, "SO_SNDTIMEO", opt_str_time};
    item[m++] = (struct socket_opt_item) {SOL_SOCKET, SO_RCVTIMEO, "SO_RCVTIMEO", opt_str_time};

    return m;
}

static void print_socket_options_array(int fd, struct socket_opt_item *item, int n) {
    sock_opt opt;
    socklen_t length;

    while (n-- > 0) {
        length = sizeof(opt);
        if (getsockopt(fd, item->socket_level, item->socket_option, &opt, &length)) {
            ALOGE("getsockopt failure(%s)", strerror(errno));
        } else {
            ALOGE("%s", item->opt_str_method(item, &opt));
        }
        item++;
    }
}

/**
 * Print default socket options.
 */
void chapter7_sock_opt_main_2(int argc, char **argv) {
    struct socket_opt_item buf[120];
    int n;
    int fd;

    n = prepare_socket_level_options(buf, 120);
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        ALOGE("create socket Fatal(%s)", strerror(errno));
        exit(-1);
    }
    print_socket_options_array(fd, buf, n);
    close(fd);
}

static inline int enable_socket_option(int fd, int option, int enabled) {
    return setsockopt(fd, SOL_SOCKET, option, (void*)&enabled, sizeof(enabled));
}

/**
 * SO_BROADCAST
 */
static inline int enable_socket_broadcast(int fd, int enabled) {
    return enable_socket_option(fd, SO_BROADCAST, enabled);
}

/**
 * SO_DEBUG
 */
static inline int enable_socket_debug(int fd, int enabled) {
    return enable_socket_option(fd, SO_DEBUG, enabled);
}

/**
 * 不要通过网关发送数据,直接发送数据到直接连接的HOST
 */
static inline int eanble_socket_dontroute(int fd, int enabled) {
    return enable_socket_option(fd, SO_DONTROUTE, enabled);
}

/**
 * SO_ERROR用来获取so_error错误变量，这个变量只能被获取不能被app设置。
 */
static inline int get_socket_error(int fd, int *so_error) {
    socklen_t length;
    length = sizeof(*so_error);
    return getsockopt(fd, SOL_SOCKET, SO_ERROR, so_error, &length);
}

/**
 * 设置SO_LINGER参数
 */
static inline int enable_socket_linger(int fd, int enabled) {
    socklen_t length;
    length = sizeof(struct linger);
    struct linger item;

    if (enabled) {
        item = _socket_linger;
    } else {
        item.l_linger = 0;
    }
    return setsockopt(fd, SOL_SOCKET, SO_LINGER, &item, length);
}
