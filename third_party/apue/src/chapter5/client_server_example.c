#include "chapter5/client_server_example.h"

#include <limits.h>
#include <pthread.h>

#define LOG_TAG "client_server_example"
#include "utils/net.h"
#include "utils/log.h"
#include "utils/apue.h"

void client_server_example_main(int argc, char **argv) {
    client_server_example_main_1(argc,argv);
}

static void client_process_main(int argc, char **argv);
static void server_process_main(int argc, char **argv);
static void server_connect_main(int fd);

static void install_signal_handler();
static void install_sigpipe_handler();

void client_server_example_main_1(int argc, char **argv) {
    choose_process_main_routing(argc, argv,
        &server_process_main,
        &client_process_main);
}

static void fork_child_service_process(int fd, int conn) {
    pid_t pid;

    if (!(pid = fork())) {
        close(fd);
        server_connect_main(conn);
        exit(0);
    }
    close(conn);
}

void server_process_main(int argc, char **argv) {
    int fd;
    int conn;
    struct sockaddr_storage addr;
    socklen_t length;
    char buf[MAXLINE];

    fd = tcp_listen_wildcard(AF_INET, SERVER_PORT);
    ALOGE_ALWAYSE_FATAL_IF(fd == -1, "tcp_listen_wildcard FATAL(%s)", strerror(errno));
    install_signal_handler();

    for (;;) {
        length = sizeof(addr);
        if ((conn = accept(fd, (SA*) &addr, &length)) == -1) {
            if (errno != EINTR) {
                ALOGE("accept failure(%s)", strerror(errno));
            }
            continue;
        }
        ALOGI("start connection: %s", sock_ntop((SA*) &addr, length, buf, MAXLINE));
        fork_child_service_process(fd, conn);
    }
}

void client_process_main(int argc, char **argv) {
    int fd;

    if ((fd = create_local_connection(SERVER_PORT)) == -1) {
        ALOGE("create_local_connection FATAL(%s)", strerror(errno));
        return;
    }
    install_sigpipe_handler();
    client_echo(fd);
    close(fd);
}

/**
 * 服务端的服务程序设计
 */
void server_connect_main(int fd) {
    install_sigpipe_handler();
    server_echo(fd);
    close(fd);
}

static void my_signal_handler(int signo, siginfo_t *info, void *context) {
    ALOGE("my_signal_handler: %d", signo);
}

static void *zombie_children_cleanup(void *args) {
    sigset_t wait_mask;

    pthread_sigmask(SIG_BLOCK, NULL, &wait_mask);
    sigdelset(&wait_mask, SIGCHLD);

    for (;;) {
        sigsuspend(&wait_mask);
        exhaust_all_waitpid_events();
    }

    return NULL;
}

void install_signal_handler() {
    struct sigaction action;
    sigset_t mask;
    pthread_t tid;
    int r;

    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    action.sa_sigaction = &my_signal_handler;
    sigemptyset(&action.sa_mask);
    sigaction(SIGCHLD, &action, NULL);

    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    pthread_sigmask(SIG_BLOCK, &mask, NULL);

    r = pthread_create(&tid, NULL, &zombie_children_cleanup, NULL);
    ALOGE_ALWAYSE_FATAL_IF(r, "pthread_create FATAL(%s)", strerror(r));
}

/**
 * 设置SIGPIPE的处理方法
 */
void install_sigpipe_handler() {
    struct sigaction action;
    int r;

    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO | SA_RESTART;
    sigemptyset(&action.sa_mask);
    action.sa_sigaction = &my_signal_handler;
    r = sigaction(SIGPIPE, &action, NULL);
    ALOGE_ALWAYSE_FATAL_IF(r == -1, "sigaction FATAL(%s)", strerror(errno));
}

/**
 * 客户单的服务程序
 */
void client_echo(int fd) {
    struct pollfd fds[1], *ptr;
    int r;

    fds[0].fd = fd;
    fds[0].events = 0;
    fds[0].revents = 0;

    for (;;) {
        if ((r = poll(fds, 1, -1)) == -1) {
            if (errno == EINTR) {
                break;
            }
            ALOGE("poll failure(%s)", strerror(errno));
            break;
        } else if (!r) {
            continue;
        }

        ptr = fds;
        if (ptr->revents & (POLLERR | POLLHUP | POLLNVAL)) {
            ALOGE("poll error: %d", ptr->revents);
            break;
        }
    }
    ALOGE("client finish echo");
}

/**
 * 服务单的服务程序
 */
void server_echo(int fd) {
    char buf[MAXLINE];
    int n, count;

    close(fd);
    for (;;) {
        if ((n = read(fd, buf, MAXLINE)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("server_echo failure(%s)", strerror(errno));
            break;
        } else if (!n) {
            break;
        }

        if((count = writen(STDOUT_FILENO, buf, n)) != n) {
            ALOGE("writen failure: n=%d, count=%d, (%s)", n, count, strerror(errno));
        }
    }
    ALOGE("server finish echo");
}
