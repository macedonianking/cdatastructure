#include "advanced_socket.h"

#include <string.h>

#define LOG_TAG "advanced_socket"
#include "utils/apue.h"
#include "utils/net.h"
#include "utils/utils.h"

void advanced_socket_main(int argc, char **argv) {
    advanced_socket_main_1(argc, argv);
}

static void connect_alarm_timeout(int signo, siginfo_t *info, void *data) {
}

int create_connection_with_alarm_timeout(int fd, struct sockaddr *addr, socklen_t length,
    int seconds) {
    struct sigaction old_action, new_action;
    int r;

    memset(&new_action, 0, sizeof(new_action));
    new_action.sa_flags = SA_SIGINFO;
    new_action.sa_sigaction = &connect_alarm_timeout;
    if (sigaction(SIGALRM, &new_action, &old_action)) {
        return -1;
    }
    if (alarm(seconds) != 0) {
        r = -1;
        goto out;
    }
    if (connect(fd, addr, length) != 0) {
        if (errno == EINTR) {
            errno = ETIMEDOUT;
        }
        r = -1;
        goto out;
    }

    r = 0;
out:
    // restore SIGALRM
    sigaction(SIGALRM, &old_action, NULL);
    alarm(0);

    return r;
}

int datagram_echo_client_with_alarm_timeout(int fd, struct sockaddr *addr, socklen_t length) {
    char buf[MAXLINE];
    int r;
    struct sigaction old_action, new_action;

    new_action.sa_flags = SA_SIGINFO;
    new_action.sa_sigaction = &connect_alarm_timeout;
    if (sigaction(SIGALRM, &new_action, &old_action) == -1) {
        return -1;
    }

    r = 0;
    for (;;) {
        if (fgets(buf, MAXLINE, stdin) == NULL) {
            r = feof(stdin) ? 0 : -1;
            break;
        }

        alarm(5);
        if (sendto(fd, buf, strlen(buf), 0, addr, length) == -1) {
            if (errno == EINTR) {
                errno = ETIMEDOUT;
            }
            r = -1;
            break;
        }
        alarm(0);
    }

    alarm(0);
    sigaction(SIGALRM, &old_action, NULL);
    return r;
}

int set_socket_read_timeout(int fd, struct timeval *interval) {
    socklen_t length = sizeof(struct timeval);
    return setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, interval, length);
}

int set_socket_write_timeout(int fd, struct timeval *interval) {
    socklen_t length = sizeof(struct timeval);
    return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, interval, length);
}

void advanced_socket_main_1(int argc, char **argv) {

}
