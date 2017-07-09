#include "chapter14/record_lock.h"

#include "utils/apue.h"

void chapter14_record_lock(int argc, char **argv) {
    chapter14_record_lock_1(argc, argv);
}

static void fill_flock_entry(struct flock *ent, int lock_type) {
    memset(ent, 0, sizeof(*ent));
    ent->l_type = F_WRLCK;
    ent->l_whence = SEEK_SET;
    ent->l_start = 0;
    ent->l_len = 0;
}

static pid_t get_owner_pid(int fd, struct flock *ent) {
    if (!fcntl(fd, F_GETLK, ent) && ent->l_type != F_UNLCK) {
        return ent->l_pid;
    }
    return 0;
}

static void worker_process() {
    execlp("find", "find", ".", "-name", "*.c", NULL);
}

void chapter14_record_lock_1(int argc, char **argv) {
    char *name = "out/hello.txt";
    int fd;
    int my_errno;
    pid_t owner_pid, ret_pid;
    struct flock item;
    int status;

    if((fd = open(name, O_RDWR | O_CREAT, APUE_FILE_MODE)) == -1) {
        LOGE("can't open %s for read and write", name);
        exit(-1);
    }

    fill_flock_entry(&item, F_WRLCK);
    if (fcntl(fd, F_SETLK, &item)) {
        my_errno = errno;
        fill_flock_entry(&item, F_WRLCK);
        if (my_errno == EAGAIN && (owner_pid = get_owner_pid(fd, &item)) != -1) {
            LOGE("Trying to get write lock FATAL: owner by process(%d)", owner_pid);
        } else {
            LOGE("Trying to get write lock FATAL: %s", strerror(my_errno));
        }
        goto out;
    }

    if ((owner_pid = fork()) == -1) {
        goto out;
    } else if (!owner_pid) {
        worker_process();
        exit(0);
    }

    for (;;) {
        if ((ret_pid = waitpid(owner_pid, &status, 0)) == -1) {
            LOGE("wait child process FATAL");
            goto out;
        } else if (ret_pid == owner_pid) {
            close(fd);
            if (!WIFSTOPPED(status) && !WIFCONTINUED(status)) {
                break;
            }
        }
    }

out:
    close(fd);
}

int record_lock_wr_lock(int fd) {
    struct flock item;

    fill_flock_entry(&item, F_WRLCK);
    return !fcntl(fd, F_SETLK, &item) ? 0 : -1;
}

int record_lock_rd_lock(int fd) {
    struct flock item;

    fill_flock_entry(&item, F_RDLCK);
    return !fcntl(fd, F_SETLK, &item) ? 0 : -1;
}

int record_lock_is_rd_locked(int fd) {
    struct flock item;
    int r;

    r = -1;
    fill_flock_entry(&item, F_RDLCK);
    if (!fcntl(fd, F_GETLK, &item)) {
        r = item.l_type != F_UNLCK;
    }
    return r;
}

int record_lock_is_wr_locked(int fd) {
    struct flock item;
    int r;

    r = -1;
    fill_flock_entry(&item, F_WRLCK);
    if (!fcntl(fd, F_GETLK, &item)) {
        r = item.l_type != F_UNLCK;
    }
    return r;
}
