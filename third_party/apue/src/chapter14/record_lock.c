#include "chapter14/record_lock.h"

#include "utils/apue.h"

static int record_lock_rdlock(int fd);
static int record_lock_wrlock(int fd);
static int record_lock_unlock(int fd);

void chapter14_record_lock(int argc, char **argv) {
    chapter14_record_lock_3(argc, argv);
}

void chapter14_record_lock_1(int argc, char **argv) {
    char *name = "hello.txt";
    struct flock flock_obj;
    int fd;

    memset(&flock_obj, 0, sizeof(flock_obj));
    flock_obj.l_type = F_WRLCK;
    flock_obj.l_whence = SEEK_SET;
    flock_obj.l_start = 0;
    flock_obj.l_len = 0;

    if ((fd = open(name, O_RDWR | O_CREAT, APUE_FILE_MODE)) == -1) {
        APUE_ERR_SYS();
    }
    if (fcntl(fd, F_GETLK, &flock_obj)) {
        APUE_ERR_SYS();
    }
    fprintf(stdout, "%d\n", flock_obj.l_type == F_UNLCK);
}

void chapter14_record_lock_2(int argc, char **argv) {
    char *name = "hello.txt";
    int fd;

    if ((fd = open(name, O_RDWR | O_CREAT)) == -1) {
        APUE_ERR_SYS();
    }
    record_lock_wrlock(fd);
    record_lock_wrlock(fd);
    record_lock_rdlock(fd);
    record_lock_unlock(fd);
    record_lock_unlock(fd);
    close(fd);
}

static void chapter14_record_lock_3_prepare_file(int fd) {
    char buf[BUFSIZ] = "hello, world";
    int n;

    n = strlen(buf);
    write(fd, buf, n);
}

static void chapter14_record_lock_3_child_process(int fd) {
    struct flock item;
    int r;

    item.l_type = F_WRLCK;
    item.l_whence = SEEK_SET;
    item.l_start = 0;
    item.l_len = 0;

    r = fcntl(fd, F_SETLK, &item);
    LOGE("chapter14_record_lock_3_child_process: r=%d, error=%s", r, strerror(errno));
}

void chapter14_record_lock_3(int argc, char **argv) {
    char *name = "hello.txt";
    int fd;
    int status;
    pid_t child, ret;

    if ((fd = open(name, O_RDWR | O_CREAT)) == -1) {
        APUE_ERR_SYS();
    }
    LOGE("chapter14_record_lock_3 parent: pid=%d", getpid());
    chapter14_record_lock_3_prepare_file(fd);
    record_lock_wrlock(fd);
    if ((child = fork()) == -1) {
        APUE_ERR_SYS();
    } else if (!child) {
        LOGE("chapter14_record_lock_3 child: pid=%d", getpid());
        chapter14_record_lock_3_child_process(fd);
        exit(0);
    }

    for (;;) {
       if ((ret = waitpid(child, &status, 0)) == child) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                break;
            }
        } else if (ret == -1 && errno != EINTR) {
            LOGE("waitpid FATAL: %s", strerror(errno));
            break;
        }
    }
    close(fd);
}

int record_lock_lock_type(int fd, int lock_type) {
    struct flock item;

    memset(&item, 0, sizeof(item));
    item.l_type = lock_type;
    item.l_whence = SEEK_SET;
    item.l_start = 0;
    item.l_len = 0;

    if (fcntl(fd, F_SETLK, &item)) {
        return -1;
    }
    return 0;
}

int record_lock_rdlock(int fd) {
    return record_lock_lock_type(fd, F_RDLCK);
}

int record_lock_wrlock(int fd) {
    return record_lock_lock_type(fd, F_WRLCK);
}

int record_lock_unlock(int fd) {
    struct flock item;

    memset(&item, 0, sizeof(item));
    item.l_type = F_UNLCK;
    item.l_whence = 0;
    item.l_start = 0;
    item.l_len = 0;

    if (fcntl(fd, F_SETLK, &item)) {
        return -1;
    }
    return 0;
}
