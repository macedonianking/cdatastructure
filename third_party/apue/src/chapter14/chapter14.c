#include "chapter14/chapter14.h"

#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/types.h>
#include <unistd.h>

#include "utils/apue.h"
#include "utils/math_help.h"

#define MIN_FILE_SIZE   (4u << 20)

int chapter14_main(int argc, char **argv) {
    chapter14_5(argc, argv);
    return 0;
}

static void set_fl(int fd, int flag);
static void clr_fl(int fd, int flag);

static char globla_buffer[500000];

static int chapter14_1_read() {
    struct stat stat_obj;
    int fd;
    int n, total, seek_read_count;
    char *ptr;

    fd = STDIN_FILENO;
    if (fstat(fd, &stat_obj) || !S_ISREG(stat_obj.st_mode)) {
        return -1;
    }

    if (lseek(fd, 0, SEEK_SET) < 0) {
        return -1;
    }

    total = NARRAY(globla_buffer);
    ptr = globla_buffer;
    seek_read_count = 0;

    while (total > 0) {
        n = read(fd, ptr, total);
        if (n == 0) {
            if (seek_read_count == 0) {
                break;
            } else {
                if (lseek(fd, 0, SEEK_SET) != 0) {
                    break;
                }
                seek_read_count = 0;
                continue;
            }
        } else if (n < 0) {
            break;
        } else {
            total -= n;
            seek_read_count += n;
            ptr += n;
        }
    }

    return total == 0 ? 0 : -1;
}

void chapter14_1(int argc, char **argv) {
    int n, total;
    char *ptr;

    if (chapter14_1_read()) {
        APUE_ERR_SYS();
    }

    set_fl(STDOUT_FILENO, O_NONBLOCK);
    ptr = globla_buffer;
    total = NARRAY(globla_buffer);
    while (total > 0) {
        n = write(STDOUT_FILENO, ptr, total);
        if (n <= 0) {
            fprintf(stderr, "chapter14_1: count=%d, error=%s\n",
                n, strerror(errno));
        } else {
            total -= n;
            ptr += n;
            fprintf(stderr, "chapter14_1: count=%d\n", n);
        }
    }
    clr_fl(STDOUT_FILENO, O_NONBLOCK);
}

void set_fl(int fd, int flag) {
    int old_flags;

    old_flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, old_flags | flag);
}

void clr_fl(int fd, int flag) {
    int old_flags;

    old_flags = fcntl(fd, F_GETFL);
    old_flags &= ~flag;
    fcntl(fd, F_SETFL, old_flags);
}

static int check_meet_file_size_requirment(int fd, size_t min_size);

static inline const char *get_file_lock_type_name(int type) {
    switch (type) {
        case F_RDLCK: {
            return TOKEN_STR(F_RDLCK);
        }
        case F_WRLCK: {
            return TOKEN_STR(F_WRLCK);
        }
        case F_UNLCK: {
            return TOKEN_STR(F_UNLCK);
        }
        default: {
            return "UNKNOWN lock type";
        }
    }
}

static void chapter14_2_child_process(int fd) {
    struct flock lock_obj;

    fprintf(stdout, "child process pid=%d\n", getpid());
    lock_obj.l_type = F_WRLCK;
    lock_obj.l_whence = SEEK_SET;
    lock_obj.l_start = 0;
    lock_obj.l_len = 0;
    if (fcntl(fd, F_GETLK, &lock_obj)) {
        err_sys("child process fcntl F_GETLK FATAL");
    }
    fprintf(stdout, "child process lock type '%s'\n",
        get_file_lock_type_name(lock_obj.l_type));
    if (lock_obj.l_type != F_UNLCK) {
        fprintf(stdout, "child process test file lock hold by process pid=%d\n",
            lock_obj.l_pid);
    }
    exit(0);
}

/**
 * fcntl lock.
 */
void chapter14_2(int argc, char **argv) {
    int fd;
    int status;
    struct flock lock_obj;
    pid_t child;

    if (argc != 2) {
        fprintf(stderr, "must specify an filename\n");
        exit(-1);
    }

    if ((fd = open(argv[1], O_CREAT | O_RDWR, APUE_FILE_MODE)) == -1) {
        err_sys("can't open or create file %s\n", argv[1]);
    }

    if (check_meet_file_size_requirment(fd, MIN_FILE_SIZE)) {
        close(fd);
        exit(-1);
    }

    lock_obj.l_type = F_RDLCK;
    lock_obj.l_whence = SEEK_SET;
    lock_obj.l_start = 0;
    lock_obj.l_len = 0;
    lock_obj.l_pid = 0;
    if (fcntl(fd, F_GETLK, &lock_obj)) {
        err_sys("fcntl F_GETLK FATAL");
    }

    /**
     *
     */
    lock_obj.l_type = F_WRLCK;
    if (fcntl(fd, F_SETLK, &lock_obj)) {
        err_sys("can't put an entire lock on file");
    }
    fprintf(stdout, "pid=%d, lock entire file in F_WRLCK mode\n", getpid());
    if ((child = fork()) < 0) {
        err_sys("fork failure");
    } else if (!child) {
        chapter14_2_child_process(fd);
    }

    if (waitpid(child, &status, 0) == child && WIFEXITED(status) && !WEXITSTATUS(status)) {
        lock_obj.l_type = F_UNLCK;
        lock_obj.l_whence = SEEK_SET;
        lock_obj.l_start = 0;
        lock_obj.l_len = 0;
        if (fcntl(fd, F_SETLK, &lock_obj)) {
            err_sys("release file lock failure");
        }
    }
    close(fd);
}

int check_meet_file_size_requirment(int fd, size_t min_size) {
    off_t off;
    int in_fd;
    char buffer[BUFSIZ];
    int n, total, nread_one_time;
    int r;

    if ((off = lseek(fd, 0, SEEK_END)) == -1) {
        LOGE_V();
        return -1;
    }

    if (off >= min_size) {
        return 0;
    }

    if ((in_fd = open("/etc/passwd", O_RDONLY)) == -1) {
        return -1;
    }

    total = (int)(min_size - off);
    nread_one_time = 0;
    r = 0;
    while (total > 0 && r) {
        n = MIN(BUFSIZ, total);
        if((n = read(in_fd, buffer, n)) < 0) {
            r = -1;
        } else if (n == 0) {
            if ((!nread_one_time || lseek(in_fd, 0, SEEK_SET)) < 0) {
                r = -1;
            } else {
                nread_one_time = 0;
            }
        } else {
            if(write(fd, buffer, n) != n) {
                r = -1;
            }
            total -= n;
            nread_one_time += n;
        }
    }
    close(in_fd);

    return r;
}

static int chapter14_3_fill_data(char *buf, int size);

void chapter14_3(int argc, char **argv) {
    long millis, now;
    char *buf, *ptr;
    int size;
    fd_set write_set;
    int n;

    size = 50 * 1024 * 1024;
    buf = (char*) malloc(size);
    if (!buf) {
        LOGE("malloc size=%d FATAL", size);
        exit(-1);
    }
    if (chapter14_3_fill_data(buf, size)) {
        LOGE("chapter14_3_fill_data FATAL");
        exit(-1);
    }

    FD_ZERO(&write_set);
    FD_SET(STDOUT_FILENO, &write_set);

    apue_set_fl(STDOUT_FILENO, O_NONBLOCK);
    ptr = buf;
    while (size > 0) {
        millis = current_time_millis();
        n = select(STDOUT_FILENO + 1, NULL, &write_set, NULL, NULL);
        now = current_time_millis();
        if (n < 0) {
            LOGW("select fail error '%s', will continue", strerror(errno));
            continue;
        } else if (n == 0) {
            LOGE("select return 0, will continue");
            continue;
        }

        if (now != millis) {
            LOGD("select wait %ld", now - millis);
        }
        if (FD_ISSET(STDOUT_FILENO, &write_set)) {
            n = write(STDOUT_FILENO, ptr, size);
            if (n == -1) {
                LOGE("write FATAL");
                fputc('\n', stdout);
                exit(-1);
            } else {
                ptr += n;
                size -= n;
            }
        }
    }
    fputc('\n', stdout);
    free(buf);

    apue_clr_fl(STDOUT_FILENO, O_NONBLOCK);
}

int chapter14_3_fill_data(char *buf, int size) {
    int fd;
    int n;
    int r, nread_one_period;

    if ((fd = open("/etc/passwd", O_RDONLY)) < 0) {
        LOGE("can't open file '/etc/passwd' for read.");
        return -1;
    }

    r = 0;
    nread_one_period = 0;
    while (size > 0 && !r) {
        if ((n = read(fd, buf, size)) == -1) {
            r = -1;
        } else if (!n) {
            if (!nread_one_period || lseek(fd, 0, SEEK_SET) < 0) {
                r = -1;
            } else {
                nread_one_period = 0;
            }
        } else {
            buf += n;
            size -= n;
            nread_one_period += size;
        }
    }

    return r;
}

/**
 * Demonstrate how to use poll function.
 * Read:POLLIN, POLLRDNORM, POLLRDBAND, POLLPRI
 * Write: POLLOUT, POLLWRNORM, POLLWRBAND
 * Exception: POLLERR, POLLHUP, POLLNVAL
 */
void chapter14_4_2(int argc, char **argv) {
    struct pollfd fd_buf[1], *ptr;
    char *buf, *out_ptr;
    int size, n, r, ret;
    long millis, now;
    int old_flags;

    apue_dup_stderr("stderr.txt");

    size = 2 * 1024 * 1024;
    buf = (char*)malloc(size);
    DCHECK(!chapter14_3_fill_data(buf, size));
    ptr = fd_buf;
    ptr->fd = STDOUT_FILENO;
    ptr->events = POLLOUT;
    ptr->revents = 0;

    apue_set_fd_flags(STDOUT_FILENO, APUE_ADD_FD_FL, O_NONBLOCK, &old_flags);
    r = 0;
    out_ptr = buf;
    while (size > 0 && !r) {
        millis = current_time_millis();
        ret = poll(fd_buf, 1, -1);
        now = current_time_millis();
        if (millis != now) {
            LOGD("poll wait %ldms", now - millis);
        }
        if (ret < 0) {
            LOGE("poll FATAL: %s", strerror(errno));
            r = -1;
        } else if (!ret) {
            LOGE("Should not be here!!!");
        } else {
            if ((ptr->revents & (POLLERR | POLLNVAL | POLLHUP))) {
                r = -1;
                LOGE("poll fd FATAL");
                continue;
            }
            if (!(ptr->revents & POLLOUT)) {
                continue;
            }

            if ((n = write(STDOUT_FILENO, out_ptr, size)) < 0) {
                LOGE("write FATAL");
                r = -1;
            } else {
                LOGI("write %d bytes", n);
                size -= n;
                out_ptr += n;
            }
        }
    }
    free(buf);
    apue_set_fd_flags(STDOUT_FILENO, APUE_SET_FD_FL, old_flags, NULL);
    fputc('\n', stdout);
    if (r) {
        exit(-1);
    }
}

void chapter14_5(int argc, char **argv) {
    int fd;
    int old_fd_flags;
    int r, n;
    struct pollfd fd_buf[1], *ptr;
    char buf[BUFSIZ];

    fd = STDOUT_FILENO;
    apue_set_fd_flags(fd, APUE_ADD_FD_FL, O_NONBLOCK, &old_fd_flags);

    /**
     * Initialize the fd_buf array.
     */
    ptr = fd_buf;
    ptr->fd = fd;
    ptr->events = POLLPRI | POLLIN | POLLRDBAND | POLLRDNORM;
    ptr->revents = 0;

    for (;;) {
        if ((r = poll(fd_buf, 1, -1)) == -1) {
            LOGE("error=%s", strerror(errno));
        } else if (!r) {
            LOGE("poll return 0");
        } else {
            if (ptr->revents & (POLLERR | POLLHUP | POLLNVAL)) {
                LOGE("ptr->revents & (POLLERR | POLLHUP | POLLNVAL)");
            } else {
                while ((n = read(ptr->fd, buf, BUFSIZ)) > 0) {
                    // DO NOTHING
                }
            }
        }
    }
    apue_set_fd_flags(fd, APUE_SET_FD_FL, old_fd_flags, NULL);
}
