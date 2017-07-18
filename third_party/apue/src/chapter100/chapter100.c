#include "chapter100/chapter100.h"

#include <stdio.h>
#include <stdlib.h>

#define LOG_TAG "chapter100"
#include "thread/thread_looper.h"
#include "utils/log.h"

static void main_thread_handler_dispatch_message(message_t *msg) {
    ALOGE("main_thread_handler_dispatch_message");
}

static void main_thread_handler_dispatch_message_dtor(message_t *msg) {
    ALOGE("main_thread_handler_dispatch_message_dtor");
}

handler_t *g_main_handler = NULL;

int chapter100_main(int argc, char **argv) {
    message_t *msg;

    thread_looper_prepare(0);
    if (open_thread_handler(NULL, &g_main_handler)) {
        return 0;
    }

    // initial g_main_handler
    g_main_handler->dispatch_message = main_thread_handler_dispatch_message;
    g_main_handler->dispatch_message_dtor = main_thread_handler_dispatch_message_dtor;

    // send initial message
    msg = thread_handler_obtain_message(g_main_handler, 0, 0, 0, NULL);
    thread_handler_send_message_delayed(g_main_handler, msg, 10 * MILLIS_IN_NANOS);
    thread_looper_loop();
    return 0;
}
