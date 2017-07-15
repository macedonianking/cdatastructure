#ifndef chapter_src_utils_log_h
#define chapter_src_utils_log_h

#include <stdio.h>
#include <stdarg.h>

#ifndef LOG_TAG
#define LOG_TAG __FILE__
#endif

#define __FATAL() \
    do { \
        fflush(stdout);     \
        fflush(stderr);     \
        *((int*)0) = 0xbaadbeef; \
    } while(0)
    
#define DCHECK(condition)   \
    do { \
        if (!(condition)) { \
            LOGE("DCHECK(%s) FATAL", #condition);   \
            __FATAL(); \
        } \
    } while(0)

#define DCHECK_UNREACHED()  \
    do { \
        LOGE("DCHECK_UNREACHED() FATAL");   \
        __FATAL();   \
    } while (0)

enum {
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_COUNT,
};

extern void log_impl(int level, const char *file, int line, 
    const char *format, ...) __attribute__((format(printf, 4, 5)));

extern void __vprint_log(int level, const char *tag, const char *fmt, va_list args);
extern void __print_log(int level, const char *tag, const char *fmt, ...) 
    __attribute__((format(printf, 3, 4)));

#ifdef DEBUG
#define __LOG_IMPL(level, format, ...)  \
    log_impl(level, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define __LOG_IMPL_V(level)             \
    log_impl(level, __FILE__, __LINE__, NULL)
#else
#define __LOG_IMPL(level, format...)     
#define __LOG_IMPL_V(level)     
#endif

#ifndef LOG_PRI
#define LOG_PRI(level, tag, ...) __print_log(LOG_LEVEL_##level, tag, ##__VA_ARGS__)
#endif

#define LOGI(format, ...)   __LOG_IMPL(LOG_LEVEL_INFO, format, ##__VA_ARGS__)
#define LOGW(format, ...)   __LOG_IMPL(LOG_LEVEL_WARN, format, ##__VA_ARGS__)
#define LOGD(format, ...)   __LOG_IMPL(LOG_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define LOGE(format, ...)   __LOG_IMPL(LOG_LEVEL_ERROR, format, ##__VA_ARGS__)

#define ALOGI(...)          LOG_PRI(INFO, LOG_TAG, ##__VA_ARGS__)
#define ALOGW(...)          LOG_PRI(WARN, LOG_TAG, ##__VA_ARGS__)
#define ALOGD(...)          LOG_PRI(DEBUG, LOG_TAG, ##__VA_ARGS__)
#define ALOGE(...)          LOG_PRI(ERROR, LOG_TAG, ##__VA_ARGS__)

#ifndef ALOGE_IF
#define ALOGE_IF(cond, ...) do { \
    if (cond) { \
        ALOGE(__VA_ARGS__); \
    } \
} while(0)
#endif // ALOGE_IF

#ifndef LOG_ALWAYSE_FATAL_IF
#define LOG_ALWAYSE_FATAL_IF(cond, ...) do { \
    if ((cond)) { \
        ALOGE(__VA_ARGS__);   \
        __FATAL(); \
    }   \
} while(0)
#endif

#ifndef ALOGE_ALWAYSE_FATAL_IF
#define ALOGE_ALWAYSE_FATAL_IF(cond, ...) LOG_ALWAYSE_FATAL_IF(cond, __VA_ARGS__)
#endif

#define WARN_IF(cond)   \
    do {    \
        if (cond) { \
            fprintf(stderr, "WARN" #cond "\n"); \
        }   \
    } while (0)

#define TOKEN_STR(name) #name

#endif
