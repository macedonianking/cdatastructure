#ifndef APUE_SRC_UTILS_TIME_UTIL_H
#define APUE_SRC_UTILS_TIME_UTIL_H

#include <time.h>
#include <sys/time.h>
#include <stdint.h>

#define SECOND_IN_MILLIS        1000ul
#define MINUTE_IN_MILLIS        (60 * SECOND_IN_MILLIS)
#define HOUR_IN_MILLIS          (60 * MINUTE_IN_MILLIS)
#define DAY_IN_MILLIS           (24 * HOUR_IN_MILLIS)

#define SECOND_IN_NANOS         1000000000ul
#define MILLIS_IN_NANOS         1000000ul

typedef int64_t nsec_t;
typedef int64_t millis_t;

static inline millis_t current_time_millis() {
    struct timespec tv;

    clock_gettime(CLOCK_REALTIME, &tv);
    return tv.tv_sec * (millis_t) SECOND_IN_MILLIS + tv.tv_nsec / MILLIS_IN_NANOS;
}

static inline nsec_t system_clock_nano_uptime() {
    struct timespec tv;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * (nsec_t) SECOND_IN_NANOS + tv.tv_nsec;
}

static inline millis_t current_monotonic_time_millis() {
    struct timespec tv;

    clock_gettime(CLOCK_MONOTONIC, &tv);
    return tv.tv_sec * (millis_t) SECOND_IN_MILLIS + tv.tv_nsec / MILLIS_IN_NANOS;
}

static inline void millis_to_timeval(millis_t millis, struct timeval *val) {
    val->tv_sec = millis / SECOND_IN_MILLIS;
    val->tv_usec = (millis % SECOND_IN_MILLIS) * 1000ul;
}

int get_format_time(millis_t time, char *buf, int size);

/**
* 安全的获取时间描述
*/
int my_ctime_s(millis_t millis, char *buf, int size);

#endif
