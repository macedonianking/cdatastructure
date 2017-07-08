#include "chapter14/epoll_tutorial.h"

#include <sys/epoll.h>

#include "utils/apue.h"

void epoll_tutorial_main(int argc, char **argv) {
    epoll_tutorial_main_1(argc, argv);
}

void epoll_tutorial_main_1(int argc, char **argv) {
    int epfd;

    if((epfd = epoll_create(1)) == -1) {
        LOGE("epoll_create FATAL");
        return;
    }

    close(epfd);
}

static int epoll_tutorial_main_2_handler(void *args, char *buf, int n) {
    struct epoll_event event;
    int fd;
    int r;
    int nwr;

    fd = *(int*) args;
    r = 0;
    while (n > 0 && !r) {
        if (epoll_wait(fd, &event, 1, -1) || event.events & (EPOLLERR | EPOLLHUP | EPOLLRDHUP)) {
            r = -1;
        } else if (!(events & EPOLLOUT)) {
            r = -1
        } else {
            if ((nwr = write(STDOUT_FILENO, buf, n)) <= 0) {
                r = -1;
            } else {
                buf += nwr;
                n -= nwr;
            }
        }
    }
}

void epoll_tutorial_main_2(int argc, char **argv) {

}
