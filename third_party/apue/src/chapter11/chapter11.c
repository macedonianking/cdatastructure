#include "chapter11.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "thread/thread_loop.h"
#include "utils/apue.h"
#include "utils/list.h"
#include "utils/log.h"
#include "utils/time_util.h"
#include "utils/string_ext.h"

typedef struct foo_object_t {
    int                 f_id;
    int                 f_count;
    pthread_mutex_t     f_lock;
    struct foo_object_t *f_next;
} foo_object_t;

#define HASH_SIZE       29
#define HASH_ID(id)     (((unsigned int)(id)) / HASH_SIZE)
static foo_object_t     *foo_hash[HASH_SIZE];
static pthread_mutex_t  foo_hash_lock = PTHREAD_MUTEX_INITIALIZER;

static foo_object_t     *alloc_foo_object(int id);
static void             free_foo_object(foo_object_t *obj);
static void             foo_object_acquire(foo_object_t *obj);
static void             foo_object_release(foo_object_t *obj);
static void             foo_object_wait(foo_object_t *obj);
static foo_object_t     *foo_object_find(int id);

int chapter11_main(int argc, char **argv) {
    ThreadLoopInitialize();
    chapter11_6_6(argc, argv);
    return 0;
}

void chapter11_3(int argc, char **argv) {
}

void chapter11_4(int argc, char **argv) {
}

static void chapter11_6_thread_func(void *args) {
    struct timespec wait_time;
    foo_object_t *ptr;
    int cid;

    wait_time.tv_sec = 0;
    wait_time.tv_nsec = 1 * MILLIS_IN_NANOS;

    cid = custom_thread_id();
    ptr = (foo_object_t*) args;
    for (int i = 0; i < 10000; ++i) {
        fprintf(stdout, "thread=%d, id=%d, %d\n", cid,
            ptr->f_id, i);
        nanosleep(&wait_time, NULL);
    }
    foo_object_release(ptr);
}

void chapter11_6(int argc, char **argv) {
    foo_object_t *obj, *find_obj;

    obj = alloc_foo_object(0);
    foo_object_acquire(obj);
    create_detach_thread(&chapter11_6_thread_func, obj);

    foo_object_acquire(obj);
    create_detach_thread(&chapter11_6_thread_func, obj);

    find_obj = foo_object_find(obj->f_id);
    if (find_obj) {
        fprintf(stdout, "find_obj->f_id=%d, f_count=%d\n", 
            find_obj->f_id, find_obj->f_count);
    }
    foo_object_wait(obj);
}

foo_object_t *alloc_foo_object(int id) {
    foo_object_t *ptr;
    int hash;
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

    hash = HASH_ID(ptr->f_id);
    pthread_mutex_lock(&foo_hash_lock);
    ptr->f_next = foo_hash[hash];
    foo_hash[hash] = ptr;
    pthread_mutex_unlock(&foo_hash_lock);
    return ptr;
}

void free_foo_object(foo_object_t *obj) {
    DCHECK(!obj->f_count);
    foo_object_t **pptr;

    pthread_mutex_lock(&foo_hash_lock);
    pptr = &foo_hash[HASH_ID(obj->f_id)];
    while (*pptr != obj && *pptr) {
        pptr = &(*pptr)->f_next;
    }
    if (*pptr == obj) {
        *pptr = (*pptr)->f_next;
    }
    pthread_mutex_unlock(&foo_hash_lock);
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

/**
 * foo_hash_lock first, then f_lock.
 */
foo_object_t *foo_object_find(int id) {
    foo_object_t *ptr;

    pthread_mutex_lock(&foo_hash_lock);
    ptr = foo_hash[HASH_ID(id)];
    while (ptr && ptr->f_id != id) {
        ptr = ptr->f_next;
    }
    if (ptr) {
        foo_object_acquire(ptr);
    }
    pthread_mutex_unlock(&foo_hash_lock);

    return ptr;
}

/**
 * Test pthread_mutex_timedlock
 */
void chapter11_7(int argc, char **argv) {
    pthread_mutex_t mutex_obj;
    pthread_mutexattr_t mutex_attr;
    int r;
    struct timespec time_val;
    struct tm tm_obj;
    char buffer[BUFSIZ];

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_TIMED_NP);
    r = pthread_mutex_init(&mutex_obj, &mutex_attr);
    pthread_mutexattr_destroy(&mutex_attr);

    if (r) {
       APUE_ERR_SYS();      
    }

    pthread_mutex_lock(&mutex_obj);
    clock_gettime(CLOCK_REALTIME, &time_val);
    localtime_r(&time_val.tv_sec, &tm_obj);
    strftime(buffer, BUFSIZ, "%c", &tm_obj);
    fprintf(stdout, "%s\n", buffer);

    time_val.tv_sec += 10;
    r = pthread_mutex_timedlock(&mutex_obj, &time_val);
    clock_gettime(CLOCK_REALTIME, &time_val);
    localtime_r(&time_val.tv_sec, &tm_obj);
    strftime(buffer, BUFSIZ, "%c", &tm_obj);
    fprintf(stdout, "%s\n", buffer);
    perror(strerror(r));

    pthread_mutex_unlock(&mutex_obj);
    pthread_mutex_destroy(&mutex_obj);
}

typedef struct queue_obj_t {
    list_head           q_list;
    pthread_mutex_t     q_lock;
    pthread_cond_t      q_list_cond;
    pthread_cond_t      q_count_cond;
    int                 q_count;
    int                 q_finish;
} queue_obj_t;

static queue_obj_t      *alloc_queue_obj();
static void             free_queue_obj(queue_obj_t *queue);
static void             queue_obj_acquire(queue_obj_t *queue);
static void             queue_obj_release(queue_obj_t *queue);
static void             queue_obj_wait(queue_obj_t *queue);
static void             queue_obj_enqueue_msg(queue_obj_t *queue, string_node_t *msg);
static string_node_t    *queue_obj_dequeue_msg(queue_obj_t *queue);

void chapter11_6_2(int argc, char **argv) {
    pthread_rwlock_t rwlock;

    pthread_rwlock_init(&rwlock, NULL);

    pthread_rwlock_rdlock(&rwlock);
    pthread_rwlock_rdlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);

    pthread_rwlock_wrlock(&rwlock);
    pthread_rwlock_rdlock(&rwlock);
    pthread_rwlock_unlock(&rwlock);

    pthread_rwlock_destroy(&rwlock);
}

static void chapter11_6_6_thread_function_product(void *args) {
    queue_obj_t *queue;
    char buffer[BUFSIZ];
    string_node_t *node;

    queue = (queue_obj_t*) args;
    // enqueue 10000 messages.
    for (int i = 0; i < 10000; ++i) {
        snprintf(buffer, BUFSIZ, "msg#%d", i);
        node = alloc_string_node(buffer);
        queue_obj_enqueue_msg(queue, node);
    }

    // set finish flag.
    pthread_mutex_lock(&queue->q_lock);
    queue->q_finish = 1;
    pthread_cond_signal(&queue->q_list_cond);
    pthread_mutex_unlock(&queue->q_lock);

    queue_obj_release(queue);
}

static void chapter11_6_6_thread_function_consumer(void *args) {
    queue_obj_t *queue;
    string_node_t *msg;
    int cid;

    cid = custom_thread_id();
    queue = (queue_obj_t*) args;
    for (;;) {
        msg = queue_obj_dequeue_msg(queue);
        if (msg == NULL) {
            break;
        }
        fprintf(stdout, "thread_id=%d, %s\n", cid, msg->str);
        free_string_node(msg);
    }

    queue_obj_release(queue);
}

void chapter11_6_6(int argc, char **argv) {
    queue_obj_t *queue;

    queue = alloc_queue_obj();
    // create product thread.
    queue_obj_acquire(queue);
    create_detach_thread(&chapter11_6_6_thread_function_product, queue);

    // create consumer thread.
    queue_obj_acquire(queue);
    create_detach_thread(&chapter11_6_6_thread_function_consumer, queue);

    queue_obj_wait(queue);
}

queue_obj_t *alloc_queue_obj() {
    queue_obj_t *ptr;

    ptr = (queue_obj_t*) malloc(sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->q_list);
    pthread_mutex_init(&ptr->q_lock, NULL);
    pthread_cond_init(&ptr->q_list_cond, NULL);
    pthread_cond_init(&ptr->q_count_cond, NULL);
    ptr->q_count = 1;
    ptr->q_finish = 0;

    return ptr;
}

void free_queue_obj(queue_obj_t *ptr) {
    DCHECK(list_empty(&ptr->q_list));
    pthread_cond_destroy(&ptr->q_list_cond);
    pthread_cond_destroy(&ptr->q_count_cond);
    pthread_mutex_destroy(&ptr->q_lock);
    free(ptr);
}

void queue_obj_enqueue_msg(queue_obj_t *queue, string_node_t *node) {
    pthread_mutex_lock(&queue->q_lock);
    list_add_tail(&node->node, &queue->q_list);
    pthread_cond_signal(&queue->q_list_cond);
    pthread_mutex_unlock(&queue->q_lock);
}

string_node_t *queue_obj_dequeue_msg(queue_obj_t *queue) {
    string_node_t *node;

    node = NULL;
    pthread_mutex_lock(&queue->q_lock);
    while (!queue->q_finish && list_empty(&queue->q_list)) {
        pthread_cond_wait(&queue->q_list_cond, &queue->q_lock);
    }
    if (!list_empty(&queue->q_list)) {
        node = list_entry(queue->q_list.next, string_node_t, node);
        list_del(&node->node);
    }
    pthread_mutex_unlock(&queue->q_lock);

    return node;
}

void queue_obj_acquire(queue_obj_t *queue) {
    pthread_mutex_lock(&queue->q_lock);
    ++queue->q_count;
    pthread_mutex_unlock(&queue->q_lock);
}

void queue_obj_release(queue_obj_t *queue) {
    pthread_mutex_lock(&queue->q_lock);
    --queue->q_count;
    if (!queue->q_count) {
        pthread_mutex_unlock(&queue->q_lock);
        free_queue_obj(queue);
        return;
    }

    if (queue->q_count == 1) {
        pthread_cond_signal(&queue->q_count_cond);
    }
    pthread_mutex_unlock(&queue->q_lock);
}

void queue_obj_wait(queue_obj_t *queue) {
    pthread_mutex_lock(&queue->q_lock);
    while (queue->q_count != 1) {
        pthread_cond_wait(&queue->q_count_cond, &queue->q_lock);
    }
    pthread_mutex_unlock(&queue->q_lock);
    DCHECK(queue->q_count == 1);
    queue_obj_release(queue);
}
