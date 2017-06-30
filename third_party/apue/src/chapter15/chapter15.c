#include "chapter15/chapter15.h"

#include "utils/apue.h"
#include "utils/math_help.h"
#include "thread/thread_loop.h"

int chapter15_main(int argc, char **argv) {
    chapter15_5(argc, argv);
    return 0;
}

struct chapter15_2_params {
    int read_fd;
    int write_fd;
};


/**
 * 读线程
 */
static void chapter15_2_read_and_write_pipe_in_same_process_read(void *args) {
    thread_params_t *params;
    struct chapter15_2_params *ptr;

    params = (thread_params_t*) args;
    ptr = (struct chapter15_2_params*) thread_params_getparams(params);
    apue_fd_copy(ptr->read_fd, STDOUT_FILENO);
    close(ptr->read_fd);

    LOGD("chapter15_2_read_and_write_pipe_in_same_process_read finish");
    thread_params_release(params);
}

/**
 * 写线程
 */
static void chapter15_2_read_and_write_pipe_in_same_process_write(void *args) {
    thread_params_t *params;
    struct chapter15_2_params *ptr;

    params = (thread_params_t*) args;
    ptr = (struct chapter15_2_params*) thread_params_getparams(params);
    apue_read_file_in_size("/home/lhb/.profile", ptr->write_fd, 2 * 1024 * 1024);
    close(ptr->write_fd);

    LOGD("chapter15_2_read_and_write_pipe_in_same_process_write finish");
    thread_params_release(params);
}

static void chapter15_2_read_and_write_pipe_in_same_process() {
    int pipefd[2];
    thread_params_t *params;
    struct chapter15_2_params args;

    if (pipe(pipefd)) {
        LOGE("pipe FATAL");
        exit(-1);
    }

    args.read_fd = pipefd[0];
    args.write_fd = pipefd[1];

    params = alloc_thread_params();
    thread_params_setparams(params, &args, NULL);

    thread_params_acquire(params);
    create_deamon_thread(&chapter15_2_read_and_write_pipe_in_same_process_read,
        params);

    thread_params_acquire(params);
    create_deamon_thread(&chapter15_2_read_and_write_pipe_in_same_process_write,
        params);

    thread_params_wait(params);
    LOGD("chapter15_2_read_and_write_pipe_in_same_process finish");
}

void chapter15_2_1(int argc, char **argv) {
    chapter15_2_read_and_write_pipe_in_same_process();
}

static void chapter15_2_read_and_write_pipe_in_two_process() {
    int pipe_fds[2];
    pid_t child;

    if (pipe(pipe_fds)) {
        LOGE("pipe FATAL");
        exit(-1);
    }

    if ((child = fork()) < 0) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        close(pipe_fds[0]);
        dup2(pipe_fds[1], STDOUT_FILENO);
        close(pipe_fds[1]);
        execlp("find", "find", ".", "-name", "*.c", NULL);
    } else {
        close(pipe_fds[1]);
        apue_fd_copy(pipe_fds[0], STDOUT_FILENO);
        exit(0);
    }
}

void chapter15_2_2(int argc, char **argv) {
    pid_t child;
    int pipe_fds[2], fd;
    const char *name;

    name = argc >= 2 ? argv[1] : "/home/lhb/.profile";
    if (pipe(pipe_fds)) {
        LOGE("pipe FATAL");
        exit(-1);
    }
    if ((fd = open(name, O_RDONLY)) == -1) {
        LOGE("can't open '%s' for read.", name);
        exit(-1);
    }

    if ((child = fork()) == -1) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        close(pipe_fds[1]);
        close(fd);
        dup2(pipe_fds[0], STDIN_FILENO);

        execlp("more", "more", NULL);
    } else {
        close(pipe_fds[0]);
        apue_fd_copy(fd, pipe_fds[1]);
        close(fd);
        exit(0);
    }
}

void chapter15_2(int argc, char **argv) {
    chapter15_2_2(argc, argv);
}

void chapter15_2_3(int argc, char **argv) {
    chapter15_2_read_and_write_pipe_in_two_process();
}

static int chapter15_5_prepare_fifo(const char* name) {
    struct stat stat_obj;

    if (!stat(name, &stat_obj)) {
        if (!S_ISFIFO(stat_obj.st_mode)) {
            return -1;
        }
    } else {
        if (errno != ENOENT || mkfifo(name, 0755)) {
            return -1;
        }
    }

    return 0;
}

void chapter15_5_1(int argc, char **argv) {
    const char *name;
    int fd;
    char buf[BUFSIZ];
    int n;

    name = "a.fifo";
    if (argc >= 2) {
        name = argv[1];
    }
    if (chapter15_5_prepare_fifo(name)) {
        LOGE("chapter15_5_prepare_fifo FATAL");
        exit(-1);
    }

    if((fd = open(name, O_RDONLY)) == -1) {
        LOGE("open FATAL: %s", strerror(errno));
        exit(-1);
    }

    while ((n = read(fd, buf, BUFSIZ)) != -1) {
        if (n == 0) {
            LOGD("END");
            break;
        }
    }

    if (n == -1) {
        LOGE("read error: %s", strerror(errno));
    }
    close(fd);
}

static void chapter15_5_2_child_process(const char *name) {
}

void chapter15_5_2(int argc, char **argv) {
    struct stat stat_obj;
    const char *name = "a.fifo";
    struct pollfd poll_fd;
    int fifo_fd;
    pid_t child;
    int r;

    if (fstatat(AT_FDCWD, name, &stat_obj, AT_SYMLINK_NOFOLLOW)) {
        if (errno != ENOENT) {
            exit(-1);
        }
    } else {
        if (S_ISDIR(stat_obj.st_mode)) {
            LOGE("%s is a directory", name);
            exit(-1);
        }
        if (unlink(name)) {
            LOGE("unlink %s FATAL: %s", name, strerror(errno));
            exit(-1);
        }
    }

    if (mkfifoat(AT_FDCWD, name, APUE_FILE_MODE)) {
        exit(-1);
    }
    if ((fifo_fd = open(name, O_RDONLY | O_NONBLOCK)) == -1) {
        LOGE("open %s FATAL", name);
        exit(-1);
    }

    if ((child = fork()) == -1) {
        LOGE("fork FATAL: %s", strerror(errno));
        exit(-1);
    } else if (!child) {
        LOGD("child process enter: %d", getpid());
        sleep(10);
        chapter15_5_2_child_process(name);
        LOGD("child process leave: %d", getpid());
        exit(0);
    }

    r = 0;
    poll_fd.fd = fifo_fd;
    poll_fd.events = POLLIN | POLLPRI;
    poll_fd.revents = 0;
    for (;;) {
        memset(&poll_fd, 0, sizeof(struct pollfd));

        LOGE("poll wait");
        if ((r = poll(&poll_fd, 1, -1)) == -1) {
            if (errno == EINTR) {
                LOGE("poll EINTR");
                continue;
            } else {
                break;
            }
        } else if (!r) {
            LOGE("poll return 0");
            break;
        } else {
            apue_fd_copy(fifo_fd, STDOUT_FILENO);
            break;
        }
    }
    close(fifo_fd);
}

void chapter15_5(int argc, char **argv) {
    chapter15_5_2(argc, argv);
}
