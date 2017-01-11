#ifndef MAIN_MACROS_H
#define MAIN_MACROS_H

#ifdef NDEBUG
#define fatal()
#define DCHECK(condition)
#else

#include <stdio.h>
#define fatal() \
do { \
    *(int*)(0) = 0xbaadbeef; \
} while (0)

#define DCHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d DCHECK(%s) failed\n", __FILE__, __LINE__, #condition); \
        fflush(stderr); \
        fatal(); \
    } \
} while (0)
#endif // NDEBUG

#define SWAP(lhs, rhs, tmp) \
do { \
	tmp = lhs; \
	lhs = rhs; \
	rhs = tmp; \
} while(0)

#endif // MAIN_MACROS_H
