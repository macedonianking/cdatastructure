#include "chapter12/chapter12.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "thread/thread_loop.h"
#include "utils/apue.h"
#include "utils/log.h"
#include "utils/list.h"
#include "utils/string_buffer.h"

int chapter12_main(int argc, char **argv) {
    ThreadLoopInitialize();
    chapter12_6(argc, argv);
    return 0;
}

static void *chapter12_3_thread_function(void *args) {
    fprintf(stdout, "Hello world!!!\n");
    return 0;
}

void chapter12_3(int argc, char **argv) {
    char buf[BUFSIZ];
    pthread_attr_t thread_attr;
    pthread_t thread_id;
    int r;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
    if ((r = pthread_create(&thread_id, &thread_attr, &chapter12_3_thread_function, NULL))) {
        snprintf(buf, BUFSIZ, "%s:%d ", __FILE__, __LINE__);
        perror(buf);
    }
    pthread_attr_destroy(&thread_attr);

    if (!r) {
        pthread_join(thread_id, NULL);
    }
}

void chapter12_4(int argc, char **argv) {
    pthread_mutexattr_t mutex_attr;
    int mutex_type;

    pthread_mutexattr_init(&mutex_attr);
    pthread_mutexattr_gettype(&mutex_attr, &mutex_type);
    pthread_mutexattr_destroy(&mutex_attr);

    fprintf(stdout, "mutex_type == PTHREAD_MUTEX_NORMAL:%d\n",
        mutex_type == PTHREAD_MUTEX_DEFAULT);
}

void chapter12_5(int argc, char **argv) {
}

static pthread_key_t g_thread_key;

struct thread_specified_data {
    int   thread_id;
    char  *thread_name;
};

struct thread_id_node {
    struct list_head    node;
    pthread_t           thread_id;
};

static void *chapter12_6_thread_function(void *);
static void chapter12_6_thread_specified_data_destrcutor(void *data);
static void chapter12_6_dump_thread_specific_data();

void chapter12_6(int argc, char **argv) {
    DEFINE_LIST_HEAD(thread_list);
    struct thread_id_node *node_ptr;
    pthread_t tid;

    pthread_key_create(&g_thread_key, &chapter12_6_thread_specified_data_destrcutor);
    for (int i = 0; i < 10; ++i) {
        if (pthread_create(&tid, NULL, &chapter12_6_thread_function, NULL)) {
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
    pthread_key_delete(g_thread_key);
}

void *chapter12_6_thread_function(void *args) {
    struct thread_specified_data *data_ptr;
    string_buffer_t str_buf;

    thread_loop_enqueue_thread_id(current_thread_id());
    string_buffer_init(&str_buf);
    string_buffer_printf(&str_buf, "Thread#%d", custom_thread_id());

    data_ptr = (struct thread_specified_data*) malloc(sizeof(struct thread_specified_data));
    data_ptr->thread_name = str_buf.buf;
    pthread_setspecific(g_thread_key, data_ptr);
    chapter12_6_dump_thread_specific_data();

    thread_loop_dequeue_thread_id(current_thread_id());
    return 0;
}

void chapter12_6_thread_specified_data_destrcutor(void *args) {
    struct thread_specified_data *ptr;

    ptr = (struct thread_specified_data*) args;
    free(ptr->thread_name);
    free(ptr);
}

/**
 * Dump thread specific data.
 */
void chapter12_6_dump_thread_specific_data() {
    void *data;
    struct thread_specified_data *thread_data;

    if (!(data = pthread_getspecific(g_thread_key))) {
        APUE_ERR_SYS();
    }
    thread_data = (struct thread_specified_data*)data;
    flockfile(stdout);
    fprintf(stdout, "thread_id=%d, %s\n", custom_thread_id(), thread_data->thread_name);
    funlockfile(stdout);
}
