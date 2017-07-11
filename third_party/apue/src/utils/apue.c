#include "apue.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <poll.h>

#include "utils/utils.h"
#include "utils/math_help.h"

void err_sys(const char *format, ...) {
    va_list arg_list;

    va_start(arg_list, format);
    vfprintf(stderr, format, arg_list);
    va_end(arg_list);

    perror("");
    exit(-1);
}

void err_quit(const char *format, ...) {
    va_list arg_list;

    va_start(arg_list, format);
    vfprintf(stderr, format, arg_list);
    va_end(arg_list);

    fputc('\n', stderr);
    exit(-1);
}

void err_abort() {
    perror("");
    exit(-1);
}

int apue_get_fd_flags(char *buf, int buf_size, int fd) {
    int n;
    int r;

    n = 0;
#if defined(ACC_MODE) || defined(ACC_FLAG)
#error ACC_MODE or ACC_FLAG has defined
#endif
#define ACC_MODE(mode)                              \
    case (mode): {                                   \
        n = snprintf(buf, buf_size, "%s", #mode);   \
        buf += n;                                   \
        buf_size -= n;                              \
        break;                                      \
    }
#define ACC_FLAG(mode)  \
    do {                \
        if (r & (mode)) {   \
            n = snprintf(buf, buf_size, " | %s", #mode);    \
            buf += n;       \
            buf_size -= n;  \
        }   \
    } while(0)

    r = fcntl(fd, F_GETFL, 0);
    if (r < 0) {
        return n;
    }

    switch(r & O_ACCMODE) {
        ACC_MODE(O_RDONLY)
        ACC_MODE(O_WRONLY)
        ACC_MODE(O_RDWR)
    }

    ACC_FLAG(O_APPEND);
    ACC_FLAG(O_NONBLOCK);
    ACC_FLAG(O_SYNC);
    ACC_FLAG(O_DSYNC);
    ACC_FLAG(O_RSYNC);

#undef ACC_MODE
#undef ACC_FLAG
    return n;
}

int apue_clr_fl(int fd, int flags) {
    int fd_flags;

    if ((fd_flags = fcntl(fd, F_GETFL, 0)) < 0) {
        return -1;
    }

    fd_flags &= ~flags;
    if (fcntl(fd, F_SETFL, fd_flags) < 0) {
        return -1;
    }
    return 0;
}

int apue_set_fl(int fd, int flags) {
    int fd_flags;

    if ((fd_flags = fcntl(fd, F_GETFL, 0)) < 0) {
        return -1;
    }
    fd_flags |= flags;
    if(fcntl(fd, F_SETFL, fd_flags) < 0) {
        return -1;
    }
    return 0;
}

int apue_is_dir(const char *path) {
    struct stat stat_obj;

    if(!stat(path, &stat_obj)) {
        return S_ISDIR(stat_obj.st_mode);
    }
    return 0;
}

int apue_is_file(const char *path) {
    struct stat stat_obj;

    if (!stat(path, &stat_obj)) {
        return S_ISREG(stat_obj.st_mode);
    }
    return 0;
}

int apue_dup_stderr(const char *err_file) {
    DCHECK(err_file);
    int fd;

    if((fd = open(err_file, O_RDWR | O_TRUNC | O_CREAT, APUE_FILE_MODE)) < 0) {
        return -1;
    } else {
        dup2(fd, STDERR_FILENO);
        return 0;
    }
}

int apue_set_fd_flags(int fd, int how, int in_flags, int *out_flags) {
    int r;
    int flags, new_flags;

    r = -1;
    if((flags = fcntl(fd, F_GETFL)) != -1) {
        switch(how) {
            case APUE_SET_FD_FL: {
                new_flags = in_flags;
                break;
            }
            case APUE_ADD_FD_FL: {
                new_flags = flags | in_flags;
                break;
            }
            case APUE_DEL_FD_FL: {
                new_flags = flags & ~in_flags;
                break;
            }
            default: {
                goto out;
            }
        }

        if (fcntl(fd, F_SETFL, new_flags) != -1) {
            r = 0;
            if(out_flags) {
                *out_flags = flags;
            }
        }
    }

out:
    return r;
}

void apue_default_signal_handler(int signo) {
#ifndef SIGNAL_CASE
#define SIGNAL_CASE(no) \
    case (no) : { LOGD("apue_default_signal_handler: " #no); break; }
#endif

    switch(signo) {
        SIGNAL_CASE(SIGINT)
        SIGNAL_CASE(SIGCHLD)
        SIGNAL_CASE(SIGQUIT)
        SIGNAL_CASE(SIGALRM)
        SIGNAL_CASE(SIGUSR1)
        SIGNAL_CASE(SIGUSR2)
    }

#ifdef SIGNAL_CASE
#undef SIGNAL_CASE
#endif
}

void apue_default_sigaction(int signo, siginfo_t *info, void *context) {
   #ifndef SIGNAL_CASE
#define SIGNAL_CASE(no) \
    case (no) : { LOGD("apue_default_sigaction: " #no); break; }
#endif

    switch(signo) {
        SIGNAL_CASE(SIGINT)
        SIGNAL_CASE(SIGCHLD)
        SIGNAL_CASE(SIGQUIT)
        SIGNAL_CASE(SIGALRM)
        SIGNAL_CASE(SIGUSR1)
        SIGNAL_CASE(SIGUSR2)
    }

#ifdef SIGNAL_CASE
#undef SIGNAL_CASE
#endif
}

static inline int is_fd_nonblock(int fd) {
    int flags;

    flags = fcntl(fd, F_GETFL);
    if (flags < 0) {
        return -1;
    } else {
        return !!(flags & O_NONBLOCK);
    }
}

static inline int wait_non_block_ready(int fd, int events) {
    struct pollfd wait_poll;
    int r = 0;

    memset(&wait_poll, 0, sizeof(wait_poll));
    wait_poll.fd = fd;
    wait_poll.events = events;
    wait_poll.revents = 0;

    r = -1;
    while (poll(&wait_poll, 1, -1) == 1) {
        if (wait_poll.revents & events) {
            r = 0;
            break;
        }
    }

    return r;
}

int apue_fd_copy(int in_fd, int out_fd) {
    char buf[BUFSIZ];
    int n;
    int r;

    r = 0;
    for (;;) {
        if ((n = readn(in_fd, buf, BUFSIZ)) == -1 || n == 0) {
            if (n == -1) {
                r = -1;
            }
            break;
        }
        if ((n = writen(out_fd, buf, n)) == -1 || n == 0) {
            if (n == -1) {
                r = -1;
            }
            break;
        }
    }

    return r;
}

int apue_read_file_in_size(const char* name, int out_fd, int min_size) {
    char buf[BUFSIZ];
    int n, read_file_count;
    int in_fd;
    int r;

    r = 0;
    if ((in_fd = open(name, O_RDONLY)) < 0) {
        return -1;
    }

    read_file_count = 0;
    while (!r && min_size > 0) {
        if ((n = read(in_fd, buf, MIN(BUFSIZ, min_size))) < 0) {
            r = -1;
        } else if (n == 0) {
            if (!read_file_count || lseek(in_fd, 0, SEEK_SET) != 0) {
                r = -1;
            } else {
                read_file_count = 0;
            }
        } else {
            read_file_count += n;
            if (write(out_fd, buf, n) != n) {
                r = -1;
            } else {
                min_size -= n;
            }
        }
    }

    close(in_fd);
    return r;
}

int apue_handle_file_in_size(const char *name, int min_size,
                             int (*handler)(void *, char *, int),
                             void *data) {
    char buf[BUFSIZ];
    int n, read_file_count;
    int in_fd;
    int r, ret;

    r = 0;
    if ((in_fd = open(name, O_RDONLY)) < 0) {
        return -1;
    }

    read_file_count = 0;
    while (!r && min_size > 0) {
        if ((n = read(in_fd, buf, MIN(BUFSIZ, min_size))) < 0) {
            r = -1;
        } else if (n == 0) {
            if (!read_file_count || lseek(in_fd, 0, SEEK_SET) != 0) {
                r = -1;
            } else {
                read_file_count = 0;
            }
        } else {
            read_file_count += n;
            ret = handler(data, buf, n);
            if (ret == -1) {
                r = -1;
            } else  if (!ret) {
                break;
            } else {
                min_size -= n;
            }
        }
    }

    close(in_fd);
    return r;
}

static inline ssize_t readn_nonblock(int fd, void *buf, size_t size) {
    struct pollfd wait_fd;
    ssize_t count;
    size_t remain;
    int n;
    char *cbuf = (char*) buf;

    wait_fd.fd = fd;
    wait_fd.events = POLLIN | POLLPRI;
    wait_fd.revents = 0;

    remain = size;
    count = 0;
    while (remain > 0) {
        if ((n = poll(&wait_fd, 1, -1)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            goto meet_error;
        } else if (!n) {
            continue;
        }
        if (wait_fd.events & (POLLIN | POLLPRI)) {
            if ((n = read(fd, cbuf + count, remain)) == -1) {
                if (errno == EINTR) {
                    continue;
                }
                goto meet_error;
            } else if (!n) {
                break;
            } else {
                count += n;
                remain -= n;
            }
        } else {
            goto meet_error;
        }
    }
    goto out;

meet_error:
    if (count == 0) {
        count = -1;
    }

out:
    return count;
}

ssize_t readn(int fd, void *buf, size_t size) {
    ssize_t count, n, remain;
    char *cbuf;

    if (is_fd_nonblock(fd)) {
        return readn_nonblock(fd, buf, size);
    }

    cbuf = (char*) buf;
    remain = size;
    while (remain > 0) {
        if ((n = read(fd, cbuf + count, remain)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            goto meet_error;
        } else {
            count += n;
            remain -= n;
        }
    }

    goto out;
meet_error:
    if (count == 0) {
        count = -1;
    }

out:
    return count;
}

static inline ssize_t writen_nonblock(int fd, void *buf, size_t size) {
    struct pollfd wait_fd;
    ssize_t count, remain;
    int n;
    char *cbuf;

    wait_fd.fd = fd;
    wait_fd.events = POLLOUT;
    wait_fd.revents = 0;

    cbuf = (char*) buf;
    count = 0;
    remain = size;
    while (remain > 0) {
        if ((n = poll(&wait_fd, 1, -1)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            goto meet_error;
        } else if (!n) {
            continue;
        }

        if (wait_fd.revents & POLLOUT) {
            if((n = write(fd, cbuf + count, remain)) == -1) {
                if (errno == EINTR) {
                    continue;
                }
                goto meet_error;
            } else {
                count += n;
                remain -= n;
            }
        } else {
            goto meet_error;
        }
    }
    goto out;

meet_error:
    if (count == 0) {
        count = -1;
    }

out:
    return count;
}

ssize_t writen(int fd, void *buf, size_t size) {
    ssize_t count, remain;
    int n;
    char *cbuf;

    if (is_fd_nonblock(fd)) {
        return writen_nonblock(fd, buf, size);
    }

    cbuf = (char*) buf;
    count = 0;
    remain = size;
    while (remain > 0) {
        if ((n = write(fd, cbuf + count, remain)) == -1) {
            if (errno == EINTR) {
                continue;
            }
            goto meet_error;
        } else {
            count += n;
            remain -= n;
        }
    }
    goto out;

meet_error:
    if (count == 0) {
        count = -1;
    }

out:
    return count;
}
