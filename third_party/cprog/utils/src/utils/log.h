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

#endif
