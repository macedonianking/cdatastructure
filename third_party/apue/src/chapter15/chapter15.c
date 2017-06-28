#include "chapter15/chapter15.h"

#include "utils/apue.h"
#include "utils/math_help.h"
#include "thread/thread_loop.h"

int chapter15_main(int argc, char **argv) {
    chapter15_2(argc, argv);
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

void chapter15_2(int argc, char **argv) {
    chapter15_2_read_and_write_pipe_in_same_process();
}
