#include "utils/time_util.h"

#include <stdio.h>
#include <stdlib.h>

int get_format_time(char *buf, int size) {
    struct timespec time_obj;
    struct tm tm_obj;
    int n, r;

    r = 0;
    clock_gettime(CLOCK_REALTIME, &time_obj);
    localtime_r(&time_obj.tv_sec, &tm_obj);
    n = strftime(buf, size, "%F %T", &tm_obj);
    r = n;
    buf += n;
    n = size - n;

    r += snprintf(buf, n, ".%03ld", (time_obj.tv_nsec / 1000000L));
    return r;
}
