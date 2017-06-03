#ifndef Chapter_test_utils_test_h
#define Chapter_test_utils_test_h

#include <stdio.h>

#define ADD_TEST(name, method) \
    do { \
        fputs("==================="#name"===================\n", stdout); \
        method(); \
        fputs("==================="#name"===================\n", stdout); \
    } while (0)

#endif
