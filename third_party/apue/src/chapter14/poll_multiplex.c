#include "chapter14/poll_multiplex.h"

#include <poll.h>

#include "utils/apue.h"

void poll_multiplex_main(int argc, char **argv) {
    poll_multiplex_main_1(argc, argv);
}

static void init_pollfd_array(struct pollfd *fdarray);
static void handle_rd_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *count);
static void handle_wr_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *count);

void poll_multiplex_main_1(int argc, char **argv) {
    struct pollfd fdarray[2];
    int old_flags;
    char buffer[BUFSIZ];
    int count;
    int in_end, out_end;
    int ret_count;

    if (apue_set_fd_flags(STDOUT_FILENO, APUE_SET_FD_FL, O_NONBLOCK, &old_flags)) {
        LOGE("apue_set_fd_flags FATAL");
    }

    init_pollfd_array(fdarray);
    count = 0;
    in_end = 0;
    out_end = 0;

    while (!out_end) {
        if ((ret_count = poll(fdarray, 2, -1)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            LOGE("poll FATAL: %s", strerror(errno));
            break;
        } else if (!ret_count) {
            continue;
        }

        handle_rd_data(fdarray, &in_end, &out_end, buffer, BUFSIZ, &count);
        handle_wr_data(fdarray, &in_end, &out_end, buffer, BUFSIZ, &count);
    }
}

void init_pollfd_array(struct pollfd *fdarray) {
    struct pollfd *item;

    item = fdarray;
    memset(item, 0, sizeof(*item));
    item->fd = STDIN_FILENO;
    item->events = POLLIN | POLLPRI;

    ++item;
    memset(item, 0, sizeof(*item));
    item->fd = -1;
    item->events = POLLOUT;
}

void handle_rd_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *count) {
    struct pollfd *ptr;
    int n;

    ptr = fdarray;
    if (*in_end || *count >= buf_max) {
        return;
    }
    if (ptr->fd < 0) {
        ptr->fd = STDIN_FILENO;
        return;
    }
    if (ptr->revents & (POLLIN | POLLPRI)) {
        if ((n = read(ptr->fd, buf + *count, buf_max - *count)) == -1) {
            *in_end = 1;
            ptr->fd = -1;
        } else if (!n) {
            *in_end = 1;
            ptr->fd = -1;
        } else {
            *count += n;
            if (*count >= buf_max) {
                ptr->fd = -1;
            }
        }
    } else if (ptr->revents & (POLLERR | POLLHUP | POLLNVAL)) {
        *in_end = 1;
        ptr->fd = -1;
    } else {
        // wait continuing.
    }
}

void handle_wr_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *count) {
    struct pollfd *ptr;
    int n;

    ptr = &fdarray[1];
    if (*out_end) {
        return;
    }
    if (count <= 0) {
        ptr->fd = -1;
    } else if (ptr->fd < 0) {
        ptr->fd = STDOUT_FILENO;
    } else if (ptr->revents & POLLOUT) {
        if ((n = write(ptr->fd, buf, *count)) <= 0) {
            *out_end = 1;
            ptr->fd = -1;
        } else {
            *count -= n;
            if (*count <= 0) {
                ptr->fd = -1;
            }
            ptr = fdarray;
            if (*count < buf_max && !*in_end) {
                ptr->fd = STDIN_FILENO;
            }
        }
    } else {
        *out_end = 1;
        ptr->fd = -1;
    }
}
