#include "chapter11.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "thread/thread_loop.h"
#include "utils/apue.h"
#include "utils/log.h"
#include "utils/time_util.h"

typedef struct foo_object_t {
    int                 f_id;
    int                 f_count;
    pthread_mutex_t     f_lock;
    struct foo_object_t *f_next;
} foo_object_t;

static foo_object_t     *alloc_foo_object(int id);
static void             free_foo_object(foo_object_t *obj);
static void             foo_object_acquire(foo_object_t *obj);
static void             foo_object_release(foo_object_t *obj);
static void             foo_object_wait(foo_object_t *obj);

int chapter11_main(int argc, char **argv) {
    chapter11_6(argc, argv);
    return 0;
}

void chapter11_3(int argc, char **argv) {
}

void chapter11_4(int argc, char **argv) {
}

static void chapter11_6_thread_func(void *args) {
    struct timespec wait_time;
    foo_object_t *ptr;
    pthread_t tid;

    wait_time.tv_sec = 0;
    wait_time.tv_nsec = 1 * MILLI_IN_NANOS;

    tid = pthread_self();
    ptr = (foo_object_t*) args;
    for (int i = 0; i < 10000; ++i) {
        fprintf(stdout, "thread=%lx, id=%d, %d\n", (unsigned long) tid,
            ptr->f_id, i);
        nanosleep(&wait_time, NULL);
    }
    foo_object_release(ptr);
}

void chapter11_6(int argc, char **argv) {
    foo_object_t *obj;

    obj = alloc_foo_object(0);
    foo_object_acquire(obj);
    create_detach_thread(&chapter11_6_thread_func, obj);

    foo_object_acquire(obj);
    create_detach_thread(&chapter11_6_thread_func, obj);

    foo_object_wait(obj);
}

foo_object_t *alloc_foo_object(int id) {
    foo_object_t *ptr;
    pthread_mutexattr_t mutex_attr;

    ptr = (foo_object_t*) malloc(sizeof(*ptr));
    DCHECK(ptr != NULL);

    ptr->f_id = id;
    ptr->f_next = NULL;
    ptr->f_count = 1;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE_NP);
    if (pthread_mutex_init(&ptr->f_lock, &mutex_attr)) {
        APUE_ERR_SYS();
    }
    pthread_mutexattr_destroy(&mutex_attr);
    return ptr;
}

void free_foo_object(foo_object_t *obj) {
    DCHECK(!obj->f_count);
    pthread_mutex_destroy(&obj->f_lock);
    free(obj);
}

void foo_object_acquire(foo_object_t *obj) {
    pthread_mutex_lock(&obj->f_lock);
    ++obj->f_count;
    pthread_mutex_unlock(&obj->f_lock);
}

void foo_object_release(foo_object_t *obj) {
    pthread_mutex_lock(&obj->f_lock);
    if (--obj->f_count) {
        pthread_mutex_unlock(&obj->f_lock);
        return;
    }
    pthread_mutex_unlock(&obj->f_lock);
    free_foo_object(obj);
}

void foo_object_wait(foo_object_t *obj) {
    int wait_done;
    struct timespec time_val;

    time_val.tv_sec = 0;
    time_val.tv_nsec = 10e6;

    wait_done = 0;
    while (!wait_done) {
        pthread_mutex_lock(&obj->f_lock);
        wait_done = obj->f_count == 1;
        pthread_mutex_unlock(&obj->f_lock);

        if (!wait_done) {
            nanosleep(&time_val, NULL);
        }
    }
    foo_object_release(obj);
}
