#ifndef APUE_SRC_THREAD_THRED_LOOP_H
#define APUE_SRC_THREAD_THRED_LOOP_H

#ifdef OS_LINUX
#include <pthread.h>
#endif

#ifdef OS_LINUX
typedef pthread_t   ThreadId;
#endif

ThreadId create_detach_thread(void (*start_routing)(void*), void *args);
ThreadId current_thread_id();

int is_thread_id_equal(ThreadId src_thread_id, ThreadId dst_thread_id);

int custom_thread_id();
int thread_loop_check_custom_id(ThreadId tid);

void ThreadLoopInitialize();
void ThreadLoopInitializeEnterImpl();
void ThreadLoopInitializeLeaveImpl();

void ThreadLoopGlobalLockAcquire();
void ThreadLoopGlobalLockRelease();

#endif
