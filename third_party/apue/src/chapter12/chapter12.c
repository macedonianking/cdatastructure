#include "chapter12/chapter12.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "thread/thread_loop.h"
#include "utils/apue.h"
#include "utils/log.h"
#include "utils/list.h"
#include "utils/string_buffer.h"

extern char **environ;

int chapter12_main(int argc, char **argv) {
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

pthread_barrier_t   g_barrier;
pthread_once_t      g_once_initialzie = PTHREAD_ONCE_INIT;

static const char *my_getenv(const char *name);

/**
 * Initialize only one time.
 */
static void chapter12_6_2_pthread_once_initialization() {
    int cid;

    cid = custom_thread_id();
    for (int i = 0; i < 1000; ++i) {
        fprintf(stdout, "thread_id=%d, %d\n", cid, i);
    }
}

static void chapter12_6_2_thread_function(void *args) {
    const char *value;

    pthread_once(&g_once_initialzie, &chapter12_6_2_pthread_once_initialization);
    value = my_getenv("HOME");
    if (value != NULL) {
        fprintf(stdout, "HOME=%s\n", value);
    }
    pthread_barrier_wait(&g_barrier);
}

void chapter12_6_2(int argc, char **argv) {
    pthread_barrier_init(&g_barrier, NULL, 3);
    create_deamon_thread(&chapter12_6_2_thread_function, NULL);
    create_deamon_thread(&chapter12_6_2_thread_function, NULL);
    pthread_barrier_wait(&g_barrier);
}

static pthread_once_t   g_getenv_one = PTHREAD_ONCE_INIT;
static pthread_key_t    g_genenv_key;
#define MAXSTRINGSZ     4096

static void my_getenv_destructor(void *args) {
    free(args);
}

static void my_getenv_init_once() {
    pthread_key_create(&g_genenv_key, &my_getenv_destructor);
}

const char *my_getenv(const char *name) {
    char *value;
    int len;
    char **pptr;

    pthread_once(&g_getenv_one, &my_getenv_init_once);
    value = pthread_getspecific(g_genenv_key);
    if (!value) {
        value = (char*) malloc(sizeof(char) * MAXSTRINGSZ);
        pthread_setspecific(g_genenv_key, value);
    }
    value[0] = '\0';
    len = strlen(name);

    pptr = environ;
    while (*pptr && (strncmp(*pptr, name, len) || (*pptr)[len] != '=')) {
        pptr++;
    }
    if (*pptr) {
        strncpy(value, *pptr + len + 1, MAXSTRINGSZ - 1);
        value[MAXSTRINGSZ-1] = '\0';
        return value;
    }
    return NULL;
}
