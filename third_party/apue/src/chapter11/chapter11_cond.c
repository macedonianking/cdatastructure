#include "chapter11/chapter11_cond.h"

#include "chapter11/chapter11.h"
#include "utils/apue.h"
#include "thread/thread_loop.h"


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

static void chapter11_6_6_thread_function_product(void *args) {
    thread_params_t *params;
    queue_obj_t *queue;
    char buffer[BUFSIZ];
    string_node_t *node;

    params = (thread_params_t*) args;
    queue = (queue_obj_t*) thread_params_getparams(params);
    for (int i = 0; i < 10000; ++i) {
        snprintf(buffer, BUFSIZ, "msg#%d", i);
        node = alloc_string_node(buffer);
        queue_obj_enqueue_msg(queue, node);
    }

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
