#include "utils/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>

#include "utils/utils.h"

#ifdef SYS_gettid
#define my_gettid() ((pid_t) syscall(SYS_gettid))
#else
#error "SYS_gettid unsupported"
#endif

static char *log_error_str = "IDWEU";

static inline char get_level_char(int level) {
    if (level < LOG_LEVEL_INFO || level >= LOG_LEVEL_COUNT) {
        level = LOG_LEVEL_COUNT;
    }
    return log_error_str[level];
}

static void get_log_time(char *buf, int size) {
    struct timespec time_obj;
    struct tm tm_obj;
    int n;

    clock_gettime(CLOCK_REALTIME, &time_obj);
    localtime_r(&time_obj.tv_sec, &tm_obj);
    n = strftime(buf, size, "%m-%d %H:%M:%S", &tm_obj);
    buf += n;
    n = size - n;
    snprintf(buf, n, ".%03ld", (time_obj.tv_nsec / 1000000L));
}

void log_impl(int level, const char *file, int line, const char *format, ...) {
    char buffer[128];
    va_list args;

    get_log_time(buffer, 128);
    flockfile(stderr);
    if (format == NULL) {
        fprintf(stderr, "[%c][%s][%s:%d]\n", get_level_char(level), buffer, file, line);
    } else {
        fprintf(stderr, "[%c][%s][%s:%d]", get_level_char(level), buffer, file, line);
        va_start(args, format);
        vfprintf(stderr, format, args);
        va_end(args);
        fputc('\n', stderr);
    }
    funlockfile(stderr);
}

void __print_log(int level, const char *tag, const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    __vprint_log(level, tag, fmt, args);
    va_end(args);
}

void __vprint_log(int level, const char *tag, const char *fmt, va_list args) {
    char time_buf[128];

    get_log_time(time_buf, NARRAY(time_buf));

    flockfile(stderr);
    fprintf(stderr, "%s %-5d %-5d %c %s: ", time_buf, getpid(), my_gettid(),
        get_level_char(level), tag);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    funlockfile(stderr);
}

void __android_log_impl(int level, const char *tag, const char *file, int line,
    const char *fmt, ...) {
    const char *name, *ptr;
    char time_buf[128];
    va_list args;

    name = file;
    while ((ptr = strpbrk(name, "\\/")) && ptr[1] != '\0') {
        name = ptr + 1;
    }

    get_log_time(time_buf, NARRAY(time_buf));
    flockfile(stderr);
    fprintf(stderr, "%s %-5d %-5d %c %s: ", time_buf, getpid(), my_gettid(),
        get_level_char(level), tag);
    fprintf(stderr, "[%s:%d] ", name, line);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    funlockfile(stderr);
}
