#include "chapter14/poll_multiplex.h"

#include <poll.h>

#include "utils/apue.h"

void poll_multiplex_main(int argc, char **argv) {
    poll_multiplex_main_1(argc, argv);
}

static void init_pollfd_array(struct pollfd *fdarray);
static void prepare_rd_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end);
static void prepare_wr_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end);
static void handle_rd_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end);
static void handle_wr_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end);


void poll_multiplex_main_1(int argc, char **argv) {
    struct pollfd fdarray[2];
    int old_in_flags, old_out_flags;
    char buffer[BUFSIZ];
    int start, end;
    int in_end, out_end;
    int ret_count;

    if (apue_set_fd_flags(STDIN_FILENO, APUE_ADD_FD_FL, O_NONBLOCK, &old_in_flags)
        || apue_set_fd_flags(STDOUT_FILENO, APUE_ADD_FD_FL, O_NONBLOCK, &old_out_flags)) {
        LOGE("apue_set_fd_flags FATAL");
        exit(-1);
    }

    init_pollfd_array(fdarray);
    in_end = 0;
    out_end = 0;
    start = 0;
    end = 0;

    while (!out_end) {
        prepare_rd_data(fdarray, &in_end, &out_end, buffer, BUFSIZ, &start, &end);
        prepare_wr_data(fdarray, &in_end, &out_end, buffer, BUFSIZ, &start, &end);
        if ((ret_count = poll(fdarray, 2, -1)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            LOGE("poll FATAL: %s", strerror(errno));
            break;
        } else if (!ret_count) {
            continue;
        }

        handle_rd_data(fdarray, &in_end, &out_end, buffer, BUFSIZ, &start, &end);
        handle_wr_data(fdarray, &in_end, &out_end, buffer, BUFSIZ, &start, &end);
    }

    if (old_in_flags & O_NONBLOCK) {
        apue_set_fd_flags(STDOUT_FILENO, APUE_SET_FD_FL, old_in_flags, NULL);
    }
    if (old_out_flags & O_NONBLOCK) {
        apue_set_fd_flags(STDOUT_FILENO, APUE_SET_FD_FL, old_out_flags, NULL);
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
    item->fd = STDOUT_FILENO;
    item->events = POLLOUT;
}

void prepare_rd_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end) {
    struct pollfd *ptr;
    if (*in_end) {
        return;
    }

    ptr = fdarray;
    if (*start == *end) {
        *start = *end = 0;
    }
    if (ptr->fd < 0) {
        if (*end < buf_max) {
            ptr->fd = STDIN_FILENO;
        }
    } else {
        if (*end == buf_max) {
            ptr->fd = -1;
        }
    }
}

void handle_rd_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end) {
    struct pollfd *ptr;
    int n;

    ptr = fdarray;
    if (*in_end || ptr->fd == -1) {
        return;
    }
    if (*start == *end) {
        *start = *end = 0;
    }
    if (*end >= buf_max) {
        return;
    } else if (ptr->revents & (POLLIN | POLLPRI)) {
        if ((n = read(ptr->fd, buf + *end,  buf_max - *end)) == -1) {
            if (errno != EINTR) {
                *in_end = 1;
                ptr->fd = -1;
            }
        } else if (!n) {
            *in_end = 1;
            ptr->fd = -1;
        } else {
            *end += n;
        }
    } else if (ptr->revents & (POLLERR | POLLHUP | POLLNVAL)) {
        *in_end = 1;
        ptr->fd = -1;
    }
}

void prepare_wr_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end) {
    struct pollfd *ptr;
    if (*start == *end) {
        *start = *end = 0;
    }

    ptr = &fdarray[1];
    if (*start == *end) {
        ptr->fd = -1;
    } else {
        ptr->fd = STDOUT_FILENO;
    }
}

void handle_wr_data(struct pollfd *fdarray, int *in_end, int *out_end,
    char *buf, int buf_max, int *start, int *end) {
    struct pollfd *ptr;
    int n;

    ptr = &fdarray[1];
    if (ptr->fd != -1) {
        if (ptr->revents & POLLOUT) {
            if (*start < *end) {
                if ((n = write(ptr->fd, buf + *start, *end - *start)) == -1) {
                    if (errno != EINTR) {
                        *out_end = 1;
                        ptr->fd = -1;
                    }
                } else {
                    *start += n;
                }
            }
        } else if (ptr->revents & (POLLERR | POLLHUP | POLLNVAL)) {
            *out_end = 1;
            ptr->fd = -1;
        }
    }
    if (*start == *end && *in_end) {
        *out_end = 1;
        ptr->fd = -1;
    }
}
