#include "chapter11.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "utils/apue.h"

enum {
   ThreadUI,
   ThreadIO,
   ThreadCount,
};

static pthread_t *g_thread_buf[ThreadCount] = {0};

#define UIThreadId      (*g_thread_buf[ThreadUI])
#define IOThreadId      (*g_thread_buf[ThreadIO])

static inline int in_ui_thread() {
    return !!pthread_equal(pthread_self(), UIThreadId);
}

static void check_start_io_thread();
static void check_init_main_thread();
static void dump_current_system_time();

int chapter11_main(int argc, char **argv) {
    chapter11_6(argc, argv);
    return 0;
}

static void *io_thread_func(void *data) {
    for (;;) {
        dump_current_system_time();
        sleep(1);
    }
    return NULL;
}

void chapter11_3(int argc, char **argv) {
    check_init_main_thread();
    printf("in_ui_thread=%s\n", in_ui_thread() ? "TRUE" : "FALSE");
}

void chapter11_4(int argc, char **argv) {
    check_init_main_thread();
    check_start_io_thread();
}

void check_init_main_thread() {
    if (!g_thread_buf[ThreadUI]) {
        g_thread_buf[ThreadUI] = (pthread_t*) malloc(sizeof(pthread_t));
        *g_thread_buf[ThreadUI] = pthread_self();
    }
}

void check_start_io_thread() {
    pthread_t tid;
    void *ret_data;

    if (g_thread_buf[ThreadIO]) {
        return;
    }

    if (pthread_create(&tid, NULL, &io_thread_func, NULL)) {
        APUE_ERR_SYS();
    }

    if (pthread_join(tid, &ret_data)) {
        APUE_ERR_SYS(); 
    }
}

void dump_current_system_time() {
    char buf[BUFSIZ];
    time_t now;

    time(&now);
    ctime_r(&now, buf);
    printf("%s", buf);
}

struct share_thread_data {
    pthread_mutex_t sh_mutex;
    int sh_seq;
};

static struct share_thread_data g_share_data;

static void *test_mutex_func(void *data) {
    struct share_thread_data *ptr;

    ptr = (struct share_thread_data*) data;
    for (int i = 0; i < 100000; ++i) {
        while (pthread_mutex_trylock(&ptr->sh_mutex)) {
            ;
        }
        ++ptr->sh_seq;
        fprintf(stdout, "sh_seq=%d\n", ptr->sh_seq);
        fflush(stdout);
        pthread_mutex_unlock(&ptr->sh_mutex);
    }

    return 0;
}

void chapter11_6(int argc, char **argv) {
    pthread_t tid0, tid1;

    g_share_data.sh_seq = 0;
    pthread_mutex_init(&g_share_data.sh_mutex, NULL);
    if (pthread_create(&tid0, NULL, &test_mutex_func, &g_share_data)) {
        APUE_ERR_SYS();
    }
    if (pthread_create(&tid1, NULL, &test_mutex_func, &g_share_data)) {
        APUE_ERR_SYS();
    }

    if (pthread_join(tid0, NULL)) {
        APUE_ERR_SYS();
    }
    if (pthread_join(tid1, NULL)) {
        APUE_ERR_SYS();
    }
    pthread_mutex_destroy(&g_share_data.sh_mutex);
}
