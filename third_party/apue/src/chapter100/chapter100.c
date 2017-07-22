#include "chapter100/chapter100.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define LOG_TAG "chapter100"
#include "thread/thread_looper.h"
#include "utils/log.h"

static sigset g_sig_mask;
static sigset g_sig_save;
static int g_disable_count = 0;

static inline void disable_signal() {
    if (g_disable_count++ == 0) {
        if (sigprocmask(SIG_BLOCK, &g_sig_mask, &g_sig_save)) {
            ALOGE("disable_signal failure(%s)", strerror(errno));
        }
    }
}

static inline void enable_signal() {
    if (g_disable_count-- == 0) {
        if (sigprocmask(SIG_SETMASK, &g_sig_save, NULL)) {
            ALOGE("enable_signal failure(%s)", strerror(errno));
        }
    }
}

int chapter100_main(int argc, char **argv) {
    disable_signal();
    enable_signal();
    return 0;
}
