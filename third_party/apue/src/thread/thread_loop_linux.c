#include "thread/thread_loop.h"

#include <stdlib.h>

#include "utils/apue.h"
#include "utils/log.h"

static pthread_mutex_t thread_loop_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_params_t {
    void *t_args;
    void (*t_func)(void*);
} thread_params_t;

static void *ThreadRouting(void *args) {
    thread_params_t *params;
    void (*func)(void *);

    params = (thread_params_t*) args;
    func = params->t_func;
    args = params->t_args;
    free(params);
    func(args);

    return 0;
}

ThreadId create_detach_thread(void (*start_routing)(void*), void *args) {
    thread_params_t *params;
    pthread_attr_t attr;
    pthread_t tid;

    params = (thread_params_t*) malloc(sizeof(*params));
    DCHECK(params);
    params->t_func = start_routing;
    params->t_args = args;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid, NULL, &ThreadRouting, params)) {
        APUE_ERR_SYS();
    }
    pthread_attr_destroy(&attr);
    return tid;
}

ThreadId current_thread_id() {
    return pthread_self();
}

int is_thread_id_equal(ThreadId src_thread_id, ThreadId dst_thread_id) {
    return pthread_equal(src_thread_id, dst_thread_id);
}

void ThreadLoopGlobalLockAcquire() {
    pthread_mutex_lock(&thread_loop_lock);
}

void ThreadLoopGlobalLockRelease() {
    pthread_mutex_unlock(&thread_loop_lock);
}

void ThreadLoopInitializeEnterImpl() {

}

void ThreadLoopInitializeLeaveImpl() {

}
