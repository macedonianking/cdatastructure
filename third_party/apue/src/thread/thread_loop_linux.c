#include "thread/thread_loop.h"

#include <stdlib.h>

#include "utils/apue.h"
#include "utils/log.h"

static pthread_mutex_t thread_loop_lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct thread_params_t {
    pthread_mutex_t         th_lock;
    pthread_cond_t          th_count_condition;
    int                     th_count;
    void                    *th_args;
    void                    (*th_dctor)(void*);
} thread_params_t;

typedef struct thread_arg_entry_t {
    void *t_args;
    void (*t_func)(void*);
} thread_arg_entry_t;

void ThreadRoutingClearUpCustomThreadId(void *args) {
    thread_loop_dequeue_thread_id(current_thread_id());
}

static void *ThreadRouting(void *args) {
    thread_arg_entry_t *params;
    void (*func)(void *);

    thread_loop_enqueue_thread_id(current_thread_id());
    pthread_cleanup_push(&ThreadRoutingClearUpCustomThreadId, NULL);

    params = (thread_arg_entry_t*) args;
    func = params->t_func;
    args = params->t_args;
    free(params);
    func(args);

    pthread_cleanup_pop(1);
    return 0;
}

ThreadId create_deamon_thread(void (*start_routing)(void*), void *args) {
    thread_arg_entry_t *params;
    pthread_attr_t attr;
    pthread_t tid;

    params = (thread_arg_entry_t*) malloc(sizeof(*params));
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

thread_params_t *alloc_thread_params() {
    thread_params_t *ptr;

    ptr = (thread_params_t*) malloc(sizeof(thread_params_t));
    pthread_mutex_init(&ptr->th_lock, NULL);
    pthread_cond_init(&ptr->th_count_condition, NULL);
    ptr->th_count = 1;
    ptr->th_args = NULL;
    ptr->th_dctor = NULL;

    return ptr;
}

void free_thread_params(thread_params_t *params) {
    DCHECK(!params->th_count);

    if (params->th_dctor) {
        (*params->th_dctor)(params->th_args);
    }
    params->th_args = NULL;
    pthread_mutex_destroy(&params->th_lock);
    pthread_cond_destroy(&params->th_count_condition);
    free(params);
}

void thread_params_setparams(thread_params_t *params, void *args, void (*dctor)(void *)) {
    params->th_args = args;
    params->th_dctor = dctor;
}

void *thread_params_getparams(thread_params_t *params) {
    return params->th_args;
}

void thread_params_wait(thread_params_t *params) {
    pthread_mutex_lock(&params->th_lock);
    while (params->th_count != 1) {
        pthread_cond_wait(&params->th_count_condition, &params->th_lock);
    }
    pthread_mutex_unlock(&params->th_lock);
    thread_params_release(params);
}

void thread_params_acquire(thread_params_t *params) {
    int finish;

    DCHECK(params->th_count > 0);
    finish = 0;
    for (;!finish;) {
        pthread_mutex_lock(&params->th_lock);
        if (++params->th_count > 0) {
            finish = 1;
        } else {
            --params->th_count;
        }
        pthread_mutex_unlock(&params->th_lock);
    }
}

void thread_params_release(thread_params_t *params) {
    pthread_mutex_lock(&params->th_lock);
    --params->th_count;
    if (!params->th_count) {
        pthread_mutex_unlock(&params->th_lock);
        free_thread_params(params);
    } else if (params->th_count == 1) {
        pthread_cond_broadcast(&params->th_count_condition);
        pthread_mutex_unlock(&params->th_lock);
    } else {
        pthread_mutex_unlock(&params->th_lock);
    }
}

void thread_params_lock(thread_params_t *params) {
    pthread_mutex_lock(&params->th_lock);
}

void thread_params_unlock(thread_params_t *params) {
    pthread_mutex_unlock(&params->th_lock);
}
