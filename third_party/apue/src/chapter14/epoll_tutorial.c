#include "chapter14/epoll_tutorial.h"

#include <sys/epoll.h>

#include "utils/apue.h"
#include "utils/math_help.h"

void epoll_tutorial_main(int argc, char **argv) {
    epoll_tutorial_main_2(argc, argv);
}

void epoll_tutorial_main_1(int argc, char **argv) {
    int epfd;

    if((epfd = epoll_create(1)) == -1) {
        LOGE("epoll_create FATAL");
        return;
    }

    close(epfd);
}

struct read_buf_entry {
    char    *buf;
    int     size;
    int     len;
};

static int epoll_tutorial_main_2_handler(void *args, char *buf, int n) {
    struct read_buf_entry *ent;
    int count;

    ent = (struct read_buf_entry*) args;
    if (ent->len >= ent->size) {
        return 0;
    }

    count = MIN(n, ent->size - ent->len - 1);
    memcpy(&ent->buf[ent->len], buf, count);
    ent->len += count;
    ent->buf[ent->len] = '\0';

    return ent->len < ent->size;
}

void epoll_tutorial_main_2(int argc, char **argv) {
    struct epoll_event event;
    int epfd;
    int length, remain, n;
    char *buf, *ptr;
    struct read_buf_entry ent;
    long millis, now;

    apue_set_fd_flags(STDOUT_FILENO, APUE_ADD_FD_FL, O_NONBLOCK, NULL);
    length = 10 * 1024 * 1024;
    buf = (char*) malloc(sizeof(char) * length + 1);
    if (!buf) {
        LOGE("malloc FATAL");
        exit(-1);
    }

    event.events = EPOLLOUT;
    if((epfd = epoll_create(1)) == -1) {
        LOGE("epoll_create FATAL");
        exit(-1);
    }
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, STDOUT_FILENO, &event)) {
        LOGE("epoll_ctl FATAL: %s", strerror(errno));
        close(epfd);
        exit(-1);
    }

    ent.buf = buf;
    ent.size = length + 1;
    ent.len = 0;
    if (apue_handle_file_in_size("/etc/passwd", length,
            &epoll_tutorial_main_2_handler, &ent)) {
        LOGE("apue_handle_file_in_size FATAL: %s", strerror(errno));
    }

    ptr = buf;
    remain = ent.len;
    while (remain > 0) {
        millis = current_time_millis();
        if (epoll_wait(epfd, &event, 1, -1) <= 0
            || (event.events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))) {
            LOGE("epoll_wait FATAL: %s", strerror(errno));
            break;
        }
        now = current_time_millis();
        if (now != millis) {
            LOGD("epoll_wait %ldms", now - millis);
        }
        if ((n = write(STDOUT_FILENO, ptr, remain)) <= 0) {
            LOGE("write FATAL");
            break;
        }
        ptr += n;
        remain -= n;
    }

    free(buf);
    buf = NULL;
    close(epfd);
}
