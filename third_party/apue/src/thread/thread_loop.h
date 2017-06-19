#ifndef APUE_SRC_THREAD_THRED_LOOP_H
#define APUE_SRC_THREAD_THRED_LOOP_H

#ifdef OS_LINUX
#include <pthread.h>
#endif

#ifdef OS_LINUX
typedef pthread_t   ThreadId;
#endif

#define THREAD_PARAMS_DEFAULT_DCTOR (&thread_params_default_params_dctor)

struct thread_params_t;
typedef struct thread_params_t thread_params_t;


ThreadId create_deamon_thread(void (*start_routing)(void*), void *args);
ThreadId current_thread_id();

int is_thread_id_equal(ThreadId src_thread_id, ThreadId dst_thread_id);

int custom_thread_id();
int thread_loop_enqueue_thread_id(ThreadId tid);
int thread_loop_dequeue_thread_id(ThreadId tid);

void ThreadLoopInitialize();
void ThreadLoopInitializeEnterImpl();
void ThreadLoopInitializeLeaveImpl();

void ThreadLoopGlobalLockAcquire();
void ThreadLoopGlobalLockRelease();


/**
 * Specified thread start parameters.
 */
void                thread_params_default_params_dctor(void *);
thread_params_t     *alloc_thread_params();
void                free_thread_params(thread_params_t *params);
void                thread_params_setparams(thread_params_t *params, void *args, void (*dctor)(void*));
void                *thread_params_getparams(thread_params_t *params);
void                thread_params_wait(thread_params_t *params);
void                thread_params_acquire(thread_params_t *params);
void                thread_params_release(thread_params_t *params);
void                thread_params_lock(thread_params_t *params);
void                thread_params_unlock(thread_params_t *params);

#endif
