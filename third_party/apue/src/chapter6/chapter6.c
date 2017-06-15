#include "chapter6.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include <utils/apue.h>
#include <utils/utils.h>

int chapter6_main(int argc, char **argv) {
    chapter6_10(argc, argv);
    return 0;
}

static inline void dump_tm(struct tm *tm_obj) {
    printf("year=%d, mon=%d, day=%d, hour=%d, min=%d, sec=%d\n", 
           tm_obj->tm_year, 
           tm_obj->tm_mon, 
           tm_obj->tm_mday, 
           tm_obj->tm_hour, 
           tm_obj->tm_min, 
           tm_obj->tm_sec);
}

time_t compute_timezone_offset() {
    time_t now;
    struct tm tm_obj;

    time(&now);
    gmtime_r(&now, &tm_obj);
    return now - mktime(&tm_obj);
}

static void test_clock_gettime() {
    struct timespec time_obj;
    time_t now;

    time(&now);
    printf("now=%ld\n", now);

    clock_gettime(CLOCK_REALTIME, &time_obj);
    printf("tv_sec=%ld, tv_nsec=%ld\n", time_obj.tv_sec,
        time_obj.tv_nsec);

    clock_getres(CLOCK_REALTIME, &time_obj);
    printf("tv_sec=%ld, tv_nsec=%ld\n", time_obj.tv_sec,
        time_obj.tv_nsec);

    clock_gettime(CLOCK_MONOTONIC, &time_obj);
    printf("tv_sec=%ld, tv_nsec=%ld\n", time_obj.tv_sec,
        time_obj.tv_nsec);
}

static void test_strftime() {
    char buf[BUFSIZ];
    time_t seconds;
    struct tm tm_obj;

    time(&seconds);
    localtime_r(&seconds, &tm_obj);
    strftime(buf, BUFSIZ, "%F %T %Z", &tm_obj);
    printf("%s\n", buf);
}

void chapter6_10(int argc, 
    char **argv) {
    time_t seconds;
    struct tm tm_obj;

    time(&seconds);
    localtime_r(&seconds, &tm_obj);
    dump_tm(&tm_obj);

    gmtime_r(&seconds, &tm_obj);
    dump_tm(&tm_obj);

    seconds = (time_t)0;
    localtime_r(&seconds, &tm_obj);
    printf("local_time:");
    dump_tm(&tm_obj);

    gmtime_r(&seconds, &tm_obj);
    printf("gmtime:");
    dump_tm(&tm_obj);

    test_clock_gettime();
    test_strftime();
}
