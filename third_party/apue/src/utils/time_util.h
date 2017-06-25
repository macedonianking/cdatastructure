#ifndef APUE_SRC_UTILS_TIME_UTIL_H
#define APUE_SRC_UTILS_TIME_UTIL_H

#include <time.h>

#define SECOND_IN_MILLIS        1000ul

#define SECOND_IN_NANOS         1000000000ul
#define MILLIS_IN_NANOS         1000000ul

static inline long current_time_millis() {
    struct timespec tv;

    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec * SECOND_IN_MILLIS + tv.tv_nsec / MILLIS_IN_NANOS;
}

#endif
