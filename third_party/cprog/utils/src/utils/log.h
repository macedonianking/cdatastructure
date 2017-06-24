#ifndef chapter_src_utils_log_h
#define chapter_src_utils_log_h

#include <stdio.h>

#define __FAIL() \
    do { \
        *((int*)0) = 0xbaadbeef; \
    } while(0)
    
#define DCHECK(condition)   \
    do { \
        if (!(condition)) { \
            fprintf(stderr, "%s:%d DCHECK(%s) failure\n", __FILE__, __LINE__, #condition);    \
            __FAIL(); \
        } \
    } while(0)

#define DCHECK_UNREACHED()  \
    do { \
        fprintf(stderr, "%s:%d DCHECK_UNREACHED failure\n", __FILE__, __LINE__);    \
        fflush(stderr); \
        __FAIL();   \
    } while (0)

enum {
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_COUNT,
};

extern void log_impl(int level, const char *file, int line, 
    const char *format, ...) __attribute__((format(printf, 4, 5)));

#ifdef DEBUG
#define __LOG_IMPL(level, format, ...)  \
    log_impl(level, __FILE__, __LINE__, format, __VA_ARGS__)
#define __LOG_IMPL_V(level)             \
    log_impl(level, __FILE__, __LINE__, NULL)
#else
#define __LOG_IMPL(level, format, ...)     
#endif

#define LOGI(format, ...)   __LOG_IMPL(LOG_LEVEL_INFO, format, __VA_ARGS__)
#define LOGW(format, ...)   __LOG_IMPL(LOG_LEVEL_WARN, format, __VA_ARGS__)
#define LOGD(format, ...)   __LOG_IMPL(LOG_LEVEL_DEBUG, format, __VA_ARGS__)
#define LOGE(format, ...)   __LOG_IMPL(LOG_LEVEL_ERROR, format, __VA_ARGS__)
#define LOGE_V()            __LOG_IMPL_V(LOG_LEVEL_ERROR)

#endif
