#ifndef MAIN_MACROS_H
#define MAIN_MACROS_H

void reportDcheckFailure(const char *file, int lineNumber, const char *assertion);
void reportDcheckBacktrace();

#ifdef NDEBUG
#define fatal()
#define DCHECK(condition)
#define DCHECK_NOT_REACH()
#else

#define CRASH() \
do { \
	*(int*)0 = 0xbaadbeef; \
} while(0)

#include <stdio.h>
#define fatal() \
do { \
    *(int*)(0) = 0xbaadbeef; \
} while (0)

#define DCHECK(condition) do { \
    if (!(condition)) { \
        reportDcheckFailure(__FILE__, __LINE__, #condition); \
        CRASH(); \
    } \
} while (0)

#define DCHECK_NOT_REACH() do { \
	reportDcheckFailure(__FILE__, __LINE__, "DCHECK_NOT_REACH"); \
	CRASH(); \
} while (0)
#endif // NDEBUG

#define SWAP(lhs, rhs, tmp) \
do { \
	tmp = lhs; \
	lhs = rhs; \
	rhs = tmp; \
} while(0)

#endif // MAIN_MACROS_H
