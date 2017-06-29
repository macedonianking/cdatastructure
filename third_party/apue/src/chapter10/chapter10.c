#include "chapter10/chapter10.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include "utils/apue.h"
#include "utils/log.h"
#include "utils/math_help.h"
#include "utils/string_util.h"

int chapter10_main(int argc, char **argv) {
    chapter10_10(argc, argv);
    return 0;
}

/**
 * Demonstrate how to use signal handler.
 */
void chapter10_2(int argc, char **argv) {
    if (signal(SIGUSR1, &apue_default_signal_handler) == SIG_ERR
        || signal(SIGUSR2, &apue_default_signal_handler) == SIG_ERR) {
        LOGE("can't catch SIGUSR1 or SIGUSR2");
        exit(-1);
    }
    for (;;) {
        pause();
        LOGD("pause interrupted");
    }
    exit(0);
}

void chapter10_3(int argc, char **argv) {
    char buffer[BUFSIZ];
    struct tm tm_obj;
    struct timespec time_val;

    DCHECK(signal(SIGUSR1, &apue_default_signal_handler) != SIG_ERR);
    DCHECK(signal(SIGUSR2, &apue_default_signal_handler) != SIG_ERR);

    for (;;) {
        clock_gettime(CLOCK_REALTIME, &time_val);
        localtime_r(&time_val.tv_sec, &tm_obj);
        strftime(buffer, BUFSIZ, "%c", &tm_obj);
        fprintf(stdout, "%s\n", buffer);

        time_val.tv_sec = 1;
        time_val.tv_nsec = 0;
        nanosleep(&time_val, NULL);
    }
}

void chapter10_5(int argc, char **argv) {

}

/**
 * Demonstrate how use kill api.
 */
void chapter10_9(int argc, char **argv) {
    struct timespec timeobj;
    pid_t child;
    int status;
    long millis, delta, threshold;
    int fd;

    if ((child = fork()) < 0) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        fprintf(stdout, "child process: pid=%d, pgid=%d\n",
            getpid(), getpgid(0));
        if((fd = open("/dev/null", O_WRONLY)) < 0) {
            err_sys("can't open /dev/null");
        }
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
        execlp("find", "find", "/", "*.c", NULL);
    }

    fprintf(stdout, "parent process: pid=%d, pgid=%d\n",
        getpid(), getpgid(0));
    millis = current_time_millis();
    threshold = 10 * SECOND_IN_MILLIS;
    for (;;) {
        delta = threshold - (current_time_millis() - millis);
        delta = MAX(0, delta);
        delta = MIN(delta, threshold);
        if (delta == 0) {
            break;
        }

        timeobj.tv_sec = delta / SECOND_IN_MILLIS;
        timeobj.tv_nsec = (delta % SECOND_IN_MILLIS) * MILLIS_IN_NANOS;
        nanosleep(&timeobj, NULL);
    }
    if (kill(-getpgid(0), SIGQUIT)) {
        err_sys("kill FATAL");
    }
    if(waitpid(child, &status, 0) == child) {
        if (WIFSIGNALED(status)) {
            fprintf(stdout, "child stop for signal=%d\n, core_dump=%d",
                WTERMSIG(status),
                WCOREDUMP(status));
        }
    }

    sleep(2);
    fprintf(stdout, "parent process finish\n");
}

struct chapter10_10_action {
    list_head   node;
    int         id;
    long        time;
    void        *data;
    void        (*hook)(void *data);
    void        (*cancel)(void *data);
};

static DEFINE_LIST_HEAD(g_chapter10_10_preapre_alarm_list);
static DEFINE_LIST_HEAD(g_chapter10_10_expired_alarm_list);
static pthread_spinlock_t g_chapter10_10_spinlock;

/**
 * Move expired action to expired action list.
 */
static void chapter10_10_check_expired_action() {
    struct chapter10_10_action *ptr;
    int times;
    int r;
    long millis, seconds;

    times = 0;
    while (times < 10 && (r = pthread_spin_trylock(&g_chapter10_10_spinlock))) {
        ++times;
    }

    if (r) {
        // wait 1 second.
        alarm(1);
        return;
    }

    millis = current_monotonic_time_millis();
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &g_chapter10_10_preapre_alarm_list, node) {
        if (ptr->time <= millis) {
            list_del(&ptr->node);
            list_add_tail(&ptr->node, &g_chapter10_10_expired_alarm_list);
            continue;
        }
        break;
    }
    if (!list_empty(&g_chapter10_10_preapre_alarm_list)) {
        ptr = list_entry(g_chapter10_10_preapre_alarm_list.next, struct chapter10_10_action, node);
        seconds = ptr->time - millis;
        if (seconds < 0) {
            seconds = 1;
        } else {
            seconds /= SECOND_IN_MILLIS;
        }
        seconds = MAX(1, seconds);
        alarm(seconds);
    }

    // release pthread_spinlock_t
    DCHECK(!pthread_spin_unlock(&g_chapter10_10_spinlock));
}

static void chapter10_10_signal_action(int signo, siginfo_t *info, void *context) {
    switch(signo) {
        case SIGALRM: {
            fprintf(stdout, "SIGALRM\n");
            chapter10_10_check_expired_action();
            break;
        }
    }
}

static void chapter10_10_schedule_alarm(int seconds, void (*hook)(void*), void *data) {
    sigset_t sig_set, old_sig_set;
    struct chapter10_10_action *action, *ptr;
    struct list_head *next;

    if (seconds <= 0) {
        seconds = 1;
    }

    sigemptyset(&sig_set);
    sigaddset(&sig_set, SIGALRM);
    DCHECK(!sigprocmask(SIG_BLOCK, &sig_set, &old_sig_set));
    if (!pthread_spin_lock(&g_chapter10_10_spinlock)) {
        action = (struct chapter10_10_action*) malloc(sizeof(*action));
        memset(action, 0, sizeof(*action));
        action->hook = hook;
        action->data = data;
        action->cancel = NULL;
        action->time = current_monotonic_time_millis() + seconds * SECOND_IN_MILLIS;
        next = NULL;
        LIST_FOR_EACH_ENTRY_SAFE(ptr, &g_chapter10_10_preapre_alarm_list, node) {
            if (action->time <= ptr->time) {
                next = &ptr->node;
                break;
            }
        }
        if (next == NULL) {
            next = &g_chapter10_10_preapre_alarm_list;
        }
        list_add_tail(&action->node, next);
        if (&action->node == g_chapter10_10_preapre_alarm_list.next) {
            // action is the first data to be expired.
            alarm(seconds);
        }
        pthread_spin_unlock(&g_chapter10_10_spinlock);
    } else {
        LOGE("pthread_spin_lock FATAL");
        exit(-1);
    }
    DCHECK(!sigprocmask(SIG_SETMASK, &old_sig_set, NULL));
}

static void chapter10_10_wait() {
    sigset_t mask;
    struct chapter10_10_action *ptr;

    sigemptyset(&mask);
    for (;;) {
        LIST_FOR_EACH_ENTRY_SAFE(ptr, &g_chapter10_10_expired_alarm_list, node) {
            if (ptr->hook) {
                ptr->hook(ptr->data);
            }
            list_del(&ptr->node);
            free(ptr);
        }
        if (list_empty(&g_chapter10_10_preapre_alarm_list)) {
            break;
        }
        sigsuspend(&mask);
    }
}

static void chapter10_10_callback(void *data) {
    LOGD("chapter10_10_callback");
}

void chapter10_10(int argc, char **argv) {
    sigset_t mask, old_mask;
    struct sigaction action;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &mask, &old_mask)) {
        LOGE("sigprocmask SIGALRM FATAL");
        exit(-1);
    }

    DCHECK(!pthread_spin_init(&g_chapter10_10_spinlock, PTHREAD_PROCESS_PRIVATE));
    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO;
    action.sa_handler = 0;
    action.sa_sigaction = &chapter10_10_signal_action;
    sigemptyset(&action.sa_mask);

    if (sigaction(SIGALRM, &action, NULL)) {
        LOGE("sigaction SIGALRM FATAL");
        exit(-1);
    }

    chapter10_10_schedule_alarm(1, &chapter10_10_callback, NULL);
    chapter10_10_schedule_alarm(3, &chapter10_10_callback, NULL);
    chapter10_10_wait();
}

void chapter10_11(int argc, char **argv) {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGALRM);
    fprintf(stdout, "is member:%d\n", sigismember(&mask, SIGALRM));
    sigdelset(&mask, SIGALRM);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGINT);
}

/**
 * how: SIG_SETMASK, SIG_BLOCK, SIG_UNBLOCK
 * api: sigprocmask, sigemptyset, sigfillset, sigaddset, sigdelset.
 */
void chapter10_12(int argc, char **argv) {
    sigset_t in_set;
    sigset_t out_set;

    sigemptyset(&in_set);
    sigaddset(&in_set, SIGINT);
    sigaddset(&in_set, SIGALRM);
    if (sigprocmask(SIG_BLOCK, &in_set, NULL)) {
        LOGE("sigprocmask FATAL");
    }

    if (sigprocmask(SIG_BLOCK, NULL, &out_set)) {
        LOGE("sigprocmask get FATAL");
    }

    if (sigismember(&out_set, SIGINT) && sigismember(&out_set, SIGALRM)) {
        LOGW("SIGINT and SIGALRM");
    }
}

static void chapter10_13_signal_function(int signo) {
    if (signo == SIGQUIT) {
        fprintf(stderr, "SIGQUIT\n");
    }
}

void chapter10_13(int argc, char **argv) {
    sigset_t in_set, old_set;

    DCHECK(signal(SIGQUIT, &chapter10_13_signal_function) != SIG_ERR);

    // block SIGQUIT
    sigemptyset(&in_set);
    sigaddset(&in_set, SIGQUIT);
    if (sigprocmask(SIG_BLOCK, &in_set, &old_set)) {
        LOGE("sigprocmask SIG_BLOCK SIGQUIT FATAL");
        exit(-1);
    }
    sleep(2);
    kill(getpid(), SIGQUIT);
    sleep(2);
    if(sigpending(&in_set)) {
        LOGE("sigpending FATAL");
    }
    if (sigismember(&in_set, SIGQUIT)) {
        fprintf(stdout, "SIGQUIT pending\n");
    }

    sigprocmask(SIG_SETMASK, &old_set, NULL);
    sleep(2);
}

static void chapter10_14_signal_handler(int signo) {
    switch(signo) {
        case SIGUSR1: {
            LOGD("SIGUSR1");
            break;
        }
        case SIGUSR2: {
            LOGD("SIGUSR2");
            break;
        }
        case SIGINT: {
            LOGD("SIGINT");
            break;
        }
        case SIGCLD: {
            LOGD("SIGCLD");
            break;
        }
    }
}

static void chapter10_14_test_sigcld_child() {
    struct sigaction action;

    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGCLD, &action, NULL)) {
        LOGD("sigaction SIGCLD FATAL");
        exit(-1);
    }

    for (;;) {
        pause();
        LOGD("pause: %s", strerror(errno));
    }
}

static void chapter10_14_test_sigcld_sigaction(int signo, siginfo_t *info, void *context) {
    LOGD("chapter10_14_test_sigcld_sigaction: %d", signo);
}

/**
 * Test SIGCLD signal handler message.
 */
static void chapter10_14_test_sigcld() {
    struct sigaction action, old_action;
    pid_t child, ret;
    int status;

    // Initialize the action object.
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SIG_DFL;
    action.sa_sigaction = &chapter10_14_test_sigcld_sigaction;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);

    // Install SIGCLD signal handler.
    if (sigaction(SIGCLD, &action, &old_action)) {
        LOGE("sigaction SIGCLD FATAL");
        exit(-1);
    }

    // fork child process.
    if ((child = fork()) < 0) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        chapter10_14_test_sigcld_child();
    }

    // wait child process finish.
    for (;;) {
        ret = waitpid(child, &status, WCONTINUED | WUNTRACED);
        if (ret == -1) {
            if (errno == EINTR) {
                LOGD("waitpid interrupted by signal: %s", strerror(errno));
                continue;
            } else {
                LOGE("waitpid FATAL");
                break;
            }
        }
        if (ret == child) {
            if (WIFEXITED(status) || WIFSIGNALED(status)) {
                // child process message.
                break;
            } else if (WIFSTOPPED(status)) {
                LOGD("WIFSTOPPED(status)");
            } else if (WIFCONTINUED(status)) {
                LOGD("WIFCONTINUED(status)");
            } else {
                LOGE("UNKNOWN error.");
                break;
            }
        }
    }

    // restore SIGCLD signal handler.
    if (sigaction(SIGCLD, &old_action, NULL)) {
        LOGE("sigaction restore SIGCLD handler");
        exit(-1);
    }
}

/**
 * sigaction
 * struct sigaction sa_handler, sa_mask
 */
void chapter10_14(int argc, char **argv) {
    struct timespec time_val;
    struct sigaction sig_action;

    /**
     * Update the sigaction.
     */
    memset(&sig_action, 0, sizeof(struct sigaction));
    sig_action.sa_handler = &chapter10_14_signal_handler;
    sigemptyset(&sig_action.sa_mask);
    sigaddset(&sig_action.sa_mask, SIGINT);
    sig_action.sa_flags = SA_INTERRUPT;

    /**
     * set the sigaction to SIGUSR1 & SIGUSR2
     */
    if (sigaction(SIGUSR1, &sig_action, NULL) || sigaction(SIGUSR2, &sig_action, NULL)) {
        LOGE("sigaction SIGUSR1 or SIGUSR2 FATAL");
        exit(-1);
    }

    /**
     * nanosleep is system api that can be interrupted by signal.
     */
    time_val.tv_sec = 1;
    time_val.tv_nsec = 0;
    LOGD("nanosleep enter");
    while (nanosleep(&time_val, NULL) == -1) {
        LOGD("nanosleep: error=%s", strerror(errno));
    }
    LOGD("nanosleep leave");

    sig_action.sa_handler = SIG_DFL;
    sig_action.sa_flags = 0;
    sigemptyset(&sig_action.sa_mask);
    if (sigaction(SIGUSR1, &sig_action, NULL) || sigaction(SIGUSR2, &sig_action, NULL)) {
        LOGE("sigaction restore SIGUSR1 or SIGUSR2 FATAL");
        exit(-1);
    }

    chapter10_14_test_sigcld();
}

static void chapter10_16_sigaction(int signo, siginfo_t *info, void *context) {
    switch (signo) {
        case SIGUSR1: {
            LOGD("chapter10_16_sigaction SIGUSR1");
            break;
        }
    }
}

void chapter10_16(int argc, char **argv) {
    sigset_t set, old_set, wait_set;
    struct sigaction action;
    pid_t child;
    int status;

    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    memset(&action, 0, sizeof(action));
    action.sa_flags = SA_SIGINFO;
    action.sa_handler = NULL;
    action.sa_sigaction = &chapter10_16_sigaction;
    sigemptyset(&action.sa_mask);

    if (sigprocmask(SIG_BLOCK, &set, &old_set) || sigaction(SIGUSR1, &action, NULL)) {
        LOGE("sigprocmask or sigaction FATAL");
        exit(-1);
    }

    if ((child = fork()) < 0) {
        LOGE("fork FATAL");
        exit(-1);
    } else if (!child) {
        LOGD("child process pid=%d, send SIGUSR1 to parent process pid=%d",
            getpid(), getppid());
        kill(getppid(), SIGUSR1);
        exit(-1);
    }

    sigemptyset(&wait_set);
    sigaddset(&wait_set, SIGQUIT);
    sigsuspend(&wait_set);

    if (sigprocmask(SIG_SETMASK, &old_set, NULL)) {
        LOGE("sigprocmask FATAL");
        exit(-1);
    }

    while (waitpid(child, &status, 0) != child) {
        ;
    }
    if (WIFEXITED(status)) {
        LOGD("wait exit_code=%d", WEXITSTATUS(status));
    }
}
