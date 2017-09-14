#include "chapter5/client_server_example.h"

#include <limits.h>
#include <pthread.h>

#define LOG_TAG "client_server_example"
#include "utils/net.h"
#include "utils/log.h"
#include "utils/apue.h"

struct client_echo_entity {
    int fd;
    int request_finish;
    int send_thread_finish;
};

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

static void *client_echo_send_thread(void *args) {
    struct client_echo_entity *ptr;
    char buf[MAXLINE];
    int n, count;
    int fd;
    sigset_t mask, old_mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    ptr = (struct client_echo_entity*) args;
    fd = ptr->fd;
    for (;;) {
        if((n = read(STDIN_FILENO, buf, MAXLINE)) == -1) {
            if (errno == EINTR) {
                if (ptr->request_finish) {
                    goto out;
                }
                continue;
            }
            break;
        } else if (!n) {
            shutdown(fd, SHUT_WR);
            break;
        }

        // 在写入的过程中禁止SIGUSR1导致的EINTR
        pthread_sigmask(SIG_BLOCK, &mask, &old_mask);
        count = writen(fd, buf, n);
        pthread_sigmask(SIG_SETMASK, &old_mask, NULL);
        if (count != n) {
            break;
        }
    }

out:
    ptr->send_thread_finish = 1;
    return NULL;
}

static void client_echo_recv(int fd) {
    struct pollfd fds[1], *ptr;
    char buf[MAXLINE];
    int r;
    int n, count;

    fds[0].fd = fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;

    for (;;) {
        if ((r = poll(fds, 1, -1)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            ALOGE("poll failure(%s)", strerror(errno));
            break;
        } else if (!r) {
            continue;
        }

        ptr = fds;
        if (ptr->revents & (POLLERR | POLLHUP | POLLNVAL)) {
            break;
        } else if (!(ptr->revents & POLLIN)) {
            break;
        }

        for (;;) {
            if ((n = read(fd, buf, MAXLINE)) == -1) {
                if (errno == EINTR) {
                    continue;
                } else if(errno == EAGAIN) {
                    break;
                }
                ALOGE("read FATAL(%s)", strerror(errno));
                break;
            } else if (!n) {
                goto out;
            }

            if ((count = writen(STDOUT_FILENO, buf, n)) != n) {
                goto out;
            }
        }
    }
out:
    close(fd);
}

/**
 * 客户单的服务程序
 */
void client_echo(int fd) {
    pthread_t tid;
    struct client_echo_entity entity;
    int r;
    struct timespec time_obj;

    entity.fd = fd;
    entity.request_finish = 0;
    entity.send_thread_finish = 0;
    r = pthread_create(&tid, NULL, &client_echo_send_thread, &entity);
    ALOGE_ALWAYSE_FATAL_IF(r, "pthread_create FATAL(%s)", strerror(r));

    client_echo_recv(fd);
    entity.request_finish = 1;

    // 设置为10ms
    memset(&time_obj, 0, sizeof(time_obj));
    time_obj.tv_sec = 0;
    time_obj.tv_nsec = 10 * MILLIS_IN_NANOS;
    for (;;) {
        r = pthread_kill(tid, SIGUSR1);
        if (r == ESRCH) {
            break;
        } if (!r) {
            break;
        }

        nanosleep(&time_obj, NULL);
        if (entity.send_thread_finish) {
            break;
        }
    }

    pthread_join(tid, NULL);
}

/**
 * 服务单的服务程序
 */
void server_echo(int fd) {
    char buf[MAXLINE];
    int n, count;

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

        if((count = writen(fd, buf, n)) != n) {
            ALOGE("writen failure: n=%d, count=%d, (%s)", n, count, strerror(errno));
            break;
        }
    }
    close(fd);
}
