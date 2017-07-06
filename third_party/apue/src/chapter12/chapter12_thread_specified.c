#include "chapter12/chapter12.h"

#include <pthread.h>

#include "utils/apue.h"
#include "thread/thread_loop.h"

static pthread_key_t g_thread_key;
static pthread_once_t g_thread_key_once = PTHREAD_ONCE_INIT;

struct thread_specified_data {
    int   thread_id;
    char  *thread_name;
};

struct thread_id_node {
    struct list_head    node;
    pthread_t           thread_id;
};

static void *thread_start_routine(void *);
static void thread_key_initializer();
static void thread_specified_data_dtor(void *data);

static void set_thread_name(const char *name);
static char *get_thread_name();

void chapter12_6(int argc, char **argv) {
    DEFINE_LIST_HEAD(thread_list);
    struct thread_id_node *node_ptr;
    pthread_t tid;

    for (int i = 0; i < 10; ++i) {
        if (pthread_create(&tid, NULL, &thread_start_routine, NULL)) {
            APUE_ERR_SYS();
        }

        node_ptr = (struct thread_id_node*) malloc(sizeof(*node_ptr));
        INIT_LIST_HEAD(&node_ptr->node);
        node_ptr->thread_id = tid;
        list_add_tail(&node_ptr->node, &thread_list);
    }

    while (!list_empty(&thread_list)) {
        node_ptr = list_entry(thread_list.next, struct thread_id_node, node);
        list_del(&node_ptr->node);
        if (pthread_join(node_ptr->thread_id, NULL)) {
            APUE_ERR_SYS();
        }
        free(node_ptr);
    }
}

void thread_key_initializer() {
    pthread_key_create(&g_thread_key, &thread_specified_data_dtor);
}

void *thread_start_routine(void *args) {
    string_buffer_t str_buf;
    char *name;

    thread_loop_enqueue_thread_id(current_thread_id());

    // set thread name.
    string_buffer_init(&str_buf);
    string_buffer_printf(&str_buf, "Thread#%d", custom_thread_id());
    set_thread_name(str_buf.buf);
    string_buffer_free(&str_buf);

    name = get_thread_name();
    if (name) {
        LOGD("thread_id=%d, thread_name=%s", custom_thread_id(), name);
    }

    thread_loop_dequeue_thread_id(current_thread_id());
    return 0;
}

/**
 * 线程退出后对本线程数据的释放操作
 */
void thread_specified_data_dtor(void *args) {
    struct thread_specified_data *ptr;

    ptr = (struct thread_specified_data*) args;
    free(ptr->thread_name);
    free(ptr);
}

void set_thread_name(const char *name) {
    void *ret;
    struct thread_specified_data *data;

    data = NULL;
    pthread_once(&g_thread_key_once, &thread_key_initializer);
    ret = pthread_getspecific(g_thread_key);
    if (ret) {
        data = (struct thread_specified_data*) ret;
    }
    if (data == NULL) {
        data = (struct thread_specified_data*) malloc(sizeof(*data));
        pthread_setspecific(g_thread_key, data);
        data->thread_id = custom_thread_id();
        data->thread_name = NULL;
    }
    free(data->thread_name);
    data->thread_name = NULL;
    if (name) {
        data->thread_name = s_strdup(name);
    }
}

char *get_thread_name() {
    void *ret;
    struct thread_specified_data *data;

    pthread_once(&g_thread_key_once, &thread_key_initializer);
    ret = pthread_getspecific(g_thread_key);
    if (!ret) {
        return NULL;
    }
    data = (struct thread_specified_data*) ret;
    return data->thread_name;
}
