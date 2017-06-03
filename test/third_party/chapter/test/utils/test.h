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
        int r; \
        int start, end; \
        \
        start = fprintf(stdout, "%s", name); \
        end = start + 4; \
        end = end - end % 4; \
        if (end < _TEST_PREFIX_WIDTH) { \
            end = _TEST_PREFIX_WIDTH; \
        } \
        for (int i = start; i < end; ++i) { \
            fputc(' ', stdout); \
        } \
        start = end; \
        fprintf(stdout, "===> "); \
        \
        r = (method); \
        fprintf(stdout, "[%s]\n", r ? "TRUE" : "FALSE"); \
        if (!r) { \
            exit(-1); \
        } \
    } while (0)

#endif
