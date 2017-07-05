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
#include "utils/string_util.h"

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
    chapter11_6_1(argc, argv);
    return 0;
}

void chapter11_3(int argc, char **argv) {
    pthread_t ui_tid;

    ui_tid = pthread_self();
    fprintf(stdout, "0x%lx, sizeof=%lu, equal=%d\n", ui_tid, sizeof(ui_tid),
        pthread_equal(ui_tid, ui_tid));
}

static void *chapter11_4_start_routing(void *args) {
    for (int i = 0; i < 10000; ++i) {
        fprintf(stdout, "chapter11_4_start_routing : %d\n", i);
    }
    return NULL;
}

void chapter11_4(int argc, char **argv) {
    pthread_t tid;
    int r;

    if ((r = pthread_create(&tid, NULL, &chapter11_4_start_routing, NULL))) {
        LOGE("pthread_create FATAL: %s", strerror(r));
        exit(-1);
    }
    if (pthread_join(tid, NULL)) {
        LOGE("pthread_join FATAL: %s", strerror(r));
        exit(-1);
    }
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
    create_deamon_thread(&chapter11_6_thread_func, obj);

    foo_object_acquire(obj);
    create_deamon_thread(&chapter11_6_thread_func, obj);

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

void chapter11_6_1(int argc, char **argv) {
    pthread_mutex_t mutex_obj = PTHEAD_RECURSIVE_MUTEX_INITIALIZER_NP;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
    pthread_mutex_init(&mutex_obj, &attr);
    pthread_mutexattr_destroy(&attr);
    pthread_mutex_lock(&mutex_obj);
    pthread_mutex_unlock(&mutex_obj);
    pthread_mutex_destroy(&mutex_obj);
}

typedef struct queue_obj_t {
    list_head           q_list;
    pthread_mutex_t     q_lock;
    pthread_cond_t      q_list_cond;
    int                 q_finish;
} queue_obj_t;

static queue_obj_t      *alloc_queue_obj();
static void             free_queue_obj(void *queue);
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
    thread_params_t *params;
    queue_obj_t *queue;
    char buffer[BUFSIZ];
    string_node_t *node;

    params = (thread_params_t*) args;
    queue = (queue_obj_t*) thread_params_getparams(params);
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

    thread_params_release(params);
}

static void chapter11_6_6_thread_function_consumer(void *args) {
    thread_params_t *params;
    queue_obj_t *queue;
    string_node_t *msg;
    int cid;

    cid = custom_thread_id();
    params = (thread_params_t*) args;
    queue = (queue_obj_t*) thread_params_getparams(params);
    for (;;) {
        msg = queue_obj_dequeue_msg(queue);
        if (msg == NULL) {
            break;
        }
        fprintf(stdout, "thread_id=%d, %s\n", cid, msg->str);
        free_string_node(msg);
    }

    thread_params_release(params);
}

void chapter11_6_6(int argc, char **argv) {
    struct thread_params_t *params;
    queue_obj_t *queue;

    params = alloc_thread_params();
    queue = alloc_queue_obj();
    thread_params_setparams(params, queue, &free_queue_obj);
    // create product thread.
    thread_params_acquire(params);
    create_deamon_thread(&chapter11_6_6_thread_function_product, params);

    // create consumer thread.
    thread_params_acquire(params);
    create_deamon_thread(&chapter11_6_6_thread_function_consumer, params);

    thread_params_wait(params);
}

queue_obj_t *alloc_queue_obj() {
    queue_obj_t *ptr;

    ptr = (queue_obj_t*) malloc(sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->q_list);
    pthread_mutex_init(&ptr->q_lock, NULL);
    pthread_cond_init(&ptr->q_list_cond, NULL);
    ptr->q_finish = 0;

    return ptr;
}

void free_queue_obj(void *ptr) {
    queue_obj_t *queue;

    queue = (queue_obj_t*) ptr;
    DCHECK(list_empty(&queue->q_list));
    pthread_cond_destroy(&queue->q_list_cond);
    pthread_mutex_destroy(&queue->q_lock);
    free(queue);
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

struct monotonic_params_t {
    pthread_spinlock_t      m_lock;
    int                     m_count;
};

static void free_monotonic_params(void *args) {
    struct monotonic_params_t *params;

    params = (struct monotonic_params_t*) args;
    pthread_spin_destroy(&params->m_lock);
    free(args);
}

static void chapter11_6_7_thread_func(void *args) {
    int cid;
    struct thread_params_t *params;
    struct monotonic_params_t *monotonic_params;

    cid = custom_thread_id();
    params = (thread_params_t*) args;
    monotonic_params = (struct monotonic_params_t*) thread_params_getparams(params);
    for (int i = 0; i < 10000; ++i) {
        pthread_spin_lock(&monotonic_params->m_lock);
        fprintf(stdout, "thread_id = %d, monotonic_params sequence:%05d\n",
            cid,
            monotonic_params->m_count++);
        pthread_spin_unlock(&monotonic_params->m_lock);
    }

    thread_params_release(params);
}

void chapter11_6_7(int argc, char **argv) {
    struct thread_params_t *params;
    struct monotonic_params_t *monotonic_params;

    /**
     * Prepare parameters.
     */
    params = alloc_thread_params();
    monotonic_params = (struct monotonic_params_t*) malloc(sizeof(struct monotonic_params_t));
    monotonic_params->m_count = 0;
    pthread_spin_init(&monotonic_params->m_lock, PTHREAD_PROCESS_PRIVATE);
    thread_params_setparams(params, monotonic_params, &free_monotonic_params);

    /**
     * Create one thread.
     */
    thread_params_acquire(params);
    create_deamon_thread(&chapter11_6_7_thread_func, params);

    /**
     * Create another thread.
     */
    thread_params_acquire(params);
    create_deamon_thread(&chapter11_6_7_thread_func, params);

    /**
     * Wait all finish.
     */
    thread_params_wait(params);
}

#define NUMBER_COUNT        10000
#define NTHREAD             8
#define COUNT_PER_THREAD    (NUMBER_COUNT / NTHREAD)

struct chapter11_6_8_thread_params {
    pthread_barrier_t   *th_barrier;
    int                 *th_data;
    int                 th_length;
};

static int chapter11_6_8_compare_func(const void *lhs, const void *rhs) {
    const int *lhs_ptr = (const int *) lhs;
    const int *rhs_ptr = (const int *) rhs;

    if (*lhs_ptr < *rhs_ptr) {
        return -1;
    } else if (*lhs_ptr > *rhs_ptr) {
        return 1;
    } else {
        return 0;
    }
}

static void chapter11_6_8_thread_function(void *args) {
    struct chapter11_6_8_thread_params *params;

    params = (struct chapter11_6_8_thread_params*) args;
    qsort(params->th_data, params->th_length,
        sizeof(int), &chapter11_6_8_compare_func);
    pthread_barrier_wait(params->th_barrier);
}

static void merge_nearby_buffer(int *data, int first, int second, int end) {
    int buffer[NUMBER_COUNT];
    int i, j, m;

    m = 0;
    for (i = first, j = second; i < second && j < end;) {
        if (data[i] <= data[j]) {
            buffer[m++] = data[i++];
        } else {
            buffer[m++] = data[j++];
        }
    }
    while (i < second) {
        buffer[m++] = data[i++];
    }
    while (j < end) {
        buffer[m++] = data[j++];
    }
    memcpy(data + first, buffer, sizeof(int) * (end - first));
}

static void merge_sort_impl(int *data) {
    int buffer_count;
    int n;
    int start, second, end;

    buffer_count = COUNT_PER_THREAD;
    n = NTHREAD;
    while (n != 1) {
        for (int i = 0; i < n; i += 2) {
            start = i * buffer_count;
            second = start + buffer_count;
            end = second + buffer_count;
            merge_nearby_buffer(data, start, second, end);
        }
        buffer_count <<= 1;
        n >>= 1;
    }
}

/**
 * Demonstrate how to use barrier.
 */
void chapter11_6_8(int argc, char **argv) {
    int data[NUMBER_COUNT];
    struct chapter11_6_8_thread_params thread_params[NTHREAD], *thread_params_ptr;
    int n, j, t;
    pthread_barrier_t barrier;

    srand(time(NULL));
    for (int i = 0; i < NUMBER_COUNT; ++i) {
        data[i] = i;
    }
    n = NUMBER_COUNT;
    for (int i = 0; i < NUMBER_COUNT; ++i, --n) {
        j = (rand() % n) + i;
        t = data[i];
        data[i] = data[j];
        data[j] = t;
    }

    pthread_barrier_init(&barrier, NULL, NTHREAD+1);

    for (int i = 0; i < NTHREAD; ++i) {
        thread_params_ptr = &thread_params[i];
        thread_params_ptr->th_barrier = &barrier;
        thread_params_ptr->th_data = data + COUNT_PER_THREAD * i;
        thread_params_ptr->th_length = COUNT_PER_THREAD;
        create_deamon_thread(&chapter11_6_8_thread_function, thread_params_ptr);
    }

    pthread_barrier_wait(&barrier);
    pthread_barrier_destroy(&barrier);

    merge_sort_impl(data);
    for (int i = 0; i < NUMBER_COUNT; ++i) {
        fprintf(stdout, "%04d\n", data[i]);
    }
}
