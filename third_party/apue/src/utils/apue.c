#include "apue.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <utils/utils.h>

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
