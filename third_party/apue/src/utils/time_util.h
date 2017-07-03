#ifndef APUE_SRC_UTILS_TIME_UTIL_H
#define APUE_SRC_UTILS_TIME_UTIL_H

#include <time.h>
#include <sys/time.h>

#define SECOND_IN_MILLIS        1000ul

#define SECOND_IN_NANOS         1000000000ul
#define MILLIS_IN_NANOS         1000000ul

static inline long current_time_millis() {
    struct timespec tv;

    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec * SECOND_IN_MILLIS + tv.tv_nsec / MILLIS_IN_NANOS;
}

static inline long current_monotonic_time_millis() {
    struct timespec tv;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * SECOND_IN_MILLIS + tv.tv_nsec / MILLIS_IN_NANOS;
}

static inline void millis_to_timeval(long millis, struct timeval *val) {
    val->tv_sec = millis / SECOND_IN_MILLIS;
    val->tv_usec = (millis % SECOND_IN_MILLIS) * 1000ul;
}

#endif
