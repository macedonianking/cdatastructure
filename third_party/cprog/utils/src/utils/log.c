#include "utils/log.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>

static void get_log_time(char *buf, int size) {
    struct timespec time_obj;
    struct tm tm_obj;
    int n;

    clock_gettime(CLOCK_REALTIME, &time_obj);
    localtime_r(&time_obj.tv_sec, &tm_obj);
    n = strftime(buf, size, "%F %T", &tm_obj);
    buf += n;
    n = size - n;

    snprintf(buf, n, ".%03ld", (time_obj.tv_nsec / 1000000L));
}

void log_impl(int level, const char *file, int line, const char *format, ...) {
    const char *level_str;
    char buffer[128];
    va_list args;

    switch(level) {
        case LOG_LEVEL_INFO: {
            level_str = "I";
            break;
        }
        case LOG_LEVEL_WARN: {
            level_str = "W";
            break;
        }
        case LOG_LEVEL_DEBUG: {
            level_str = "D";
            break;
        }
        case LOG_LEVEL_ERROR: {
            level_str = "E";
            break;
        }
        default: {
            level_str = "U";
            break;
        }
    }
    get_log_time(buffer, 128);
    flockfile(stderr);
    if (format == NULL) {
        fprintf(stderr, "[%s][%s][%s:%d]\n", level_str, buffer, file, line);
    } else {
        fprintf(stderr, "[%s][%s][%s:%d]", level_str, buffer, file, line);
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fputc('\n', stderr);
    }
    funlockfile(stderr);
}
