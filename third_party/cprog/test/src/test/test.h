#ifndef Chapter_test_utils_test_h
#define Chapter_test_utils_test_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ADD_TEST(name, method) \
    do { \
        int n = strlen(#name); \
        int prefix = (100 - n) / 2; \
        int suffix = 100 - n - prefix; \
        \
        for (int i = 0; i < prefix; ++i) { \
            fputc('=', stdout); \
        } \
        fputs(#name, stdout); \
        for (int i = 0; i < suffix; ++i) { \
            fputc('=', stdout); \
        } \
        fputc('\n', stdout); \
        method(); \
        for (int i = 0; i < prefix; ++i) { \
            fputc('=', stdout); \
        } \
        fputs(#name, stdout); \
        for (int i = 0; i < suffix; ++i) { \
            fputc('=', stdout); \
        } \
        fputc('\n', stdout); \
    } while (0)

#define _TEST_PREFIX_WIDTH  18

#define TEST_CASE(name, method) \
    do { \
        int __r; \
        int __start, __end; \
        \
        __start = fprintf(stdout, "%s", #name); \
        __end = __start + 4; \
        __end = __end - __end % 4; \
        if (__end < _TEST_PREFIX_WIDTH) { \
            __end = _TEST_PREFIX_WIDTH; \
        } \
        for (int i = __start; i < __end; ++i) { \
            fputc(' ', stdout); \
        } \
        __start = __end; \
        fprintf(stdout, "===> "); \
        \
        __r = (method); \
        fprintf(stdout, "[%s]\n", __r ? "TRUE" : "FALSE"); \
        if (!__r) { \
            exit(-1); \
        } \
    } while (0)

#define TEST_VOID(name, statement) TEST_CASE(name, ({statement; 1;}))

#endif
