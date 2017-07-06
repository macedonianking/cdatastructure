#include "chapter11/chapter11_rwlock.h"

#include <pthread.h>

#include "chapter11/chapter11.h"
#include "utils/apue.h"

struct factory {
    pthread_rwlock_t    f_lock;
    pthread_barrier_t   f_barrier;
    int                 f_product_finish;
    int                 f_consume_finish;
    list_head           f_list;
};

struct product {
    int                 p_productid;
    list_head           p_node;
    char                *p_message;
};

static void factory_init(struct factory *ptr);
static void factory_destroy(struct factory *ptr);

static void *factory_productor(void *args);
static void *factory_consumer(void *args);
static void *factory_observer(void *args);

static pthread_t create_deamon_thread_local(void *(*start_routine)(void*), void *args) {
    pthread_attr_t thread_attr;
    pthread_t tid;

    pthread_attr_init(&thread_attr);
    pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&tid, &thread_attr, start_routine, args)) {
        pthread_attr_destroy(&thread_attr);
        LOGE("create_deamon_thread_local FATAL");
        exit(-1);
    }
    pthread_attr_destroy(&thread_attr);
    return tid;
}

void chapter11_6_4(int argc, char **argv) {
    struct factory factory_obj;

    factory_init(&factory_obj);
    create_deamon_thread_local(&factory_productor, &factory_obj);
    create_deamon_thread_local(&factory_consumer, &factory_obj);
    create_deamon_thread_local(&factory_observer, &factory_obj);

    pthread_barrier_wait(&factory_obj.f_barrier);
    factory_destroy(&factory_obj);
}

void factory_init(struct factory *ptr) {
    pthread_rwlock_init(&ptr->f_lock, NULL);
    pthread_barrier_init(&ptr->f_barrier, NULL, 4);
    ptr->f_product_finish = 0;
    ptr->f_consume_finish = 0;
    INIT_LIST_HEAD(&ptr->f_list);
}

void factory_destroy(struct factory *ptr) {
    pthread_rwlock_destroy(&ptr->f_lock);
    pthread_barrier_destroy(&ptr->f_barrier);
}

void *factory_productor(void *args) {
    char buffer[BUFSIZ];
    struct factory *ptr;
    struct product *item;

    ptr = (struct factory*) args;
    for (int i = 0; i != 100; ++i) {
        item = (struct product*) malloc(sizeof(*item));
        INIT_LIST_HEAD(&item->p_node);
        item->p_productid = i;
        snprintf(buffer, BUFSIZ, "factory_productor message %d", i);
        item->p_message = s_strdup(buffer);

        pthread_rwlock_wrlock(&ptr->f_lock);
        list_add_tail(&item->p_node, &ptr->f_list);
        pthread_rwlock_unlock(&ptr->f_lock);
    }

    pthread_rwlock_wrlock(&ptr->f_lock);
    ptr->f_product_finish = 1;
    pthread_rwlock_unlock(&ptr->f_lock);

    pthread_barrier_wait(&ptr->f_barrier);
    return NULL;
}

void *factory_consumer(void *args) {
    struct factory *ptr;
    struct product *item;
    int finish;

    ptr = (struct factory*) args;
    finish = 0;
    while (!finish) {
        item = NULL;
        pthread_rwlock_wrlock(&ptr->f_lock);
        if (list_empty(&ptr->f_list)) {
            if (ptr->f_product_finish) {
                ptr->f_consume_finish = 1;
                finish = 1;
            }
        } else {
            item = list_entry(ptr->f_list.next, struct product, p_node);
            list_del(&item->p_node);
        }
        pthread_rwlock_unlock(&ptr->f_lock);

        if (item != NULL) {
            free(item->p_message);
            item->p_message = NULL;
            free(item);
        }
    }

    pthread_barrier_wait(&ptr->f_barrier);
    return NULL;
}

static void observer_print(struct factory*ptr, int *out_product_id) {
    struct product *item;
    int last_product_id = *out_product_id;

    LIST_FOR_EACH_ENTRY(item, &ptr->f_list, p_node) {
        if (item->p_productid > last_product_id) {
            fprintf(stdout, "%s\n", item->p_message);
            last_product_id = item->p_productid;
        }
    }

    *out_product_id = last_product_id;
}

void *factory_observer(void *args) {
    struct factory *ptr;
    int last_product_id;
    int finish;
    struct timespec time_obj;

    time_obj.tv_sec = 0;
    time_obj.tv_nsec = 1000;

    ptr = (struct factory*) args;
    last_product_id = -1;
    finish =0;
    while (!finish) {
        pthread_rwlock_rdlock(&ptr->f_lock);
        if (!list_empty(&ptr->f_list)) {
            observer_print(ptr, &last_product_id);
        } else if (ptr->f_product_finish) {
            finish = 1;
        }
        pthread_rwlock_unlock(&ptr->f_lock);
        if (!finish) {
            nanosleep(&time_obj, NULL);
        }
    }

    pthread_barrier_wait(&ptr->f_barrier);
    return NULL;
}
