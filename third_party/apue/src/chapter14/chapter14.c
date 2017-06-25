#include "chapter14/chapter14.h"

#include "utils/apue.h"
#include "utils/math_help.h"

#define MIN_FILE_SIZE   (4u << 20)

static int check_meet_file_size_requirment(int fd, size_t min_size);

int chapter14_main(int argc, char **argv) {
    chapter14_2(argc, argv);
    return 0;
}

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
