#ifndef K_DEBUG_H
#define K_DEBUG_H

#include <assert.h>

#define ASSERT(condition) assert(condition)

#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define KDEBUG

#endif // K_DEBUG_H
