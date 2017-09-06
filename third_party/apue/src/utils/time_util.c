#include "utils/time_util.h"

#include <stdio.h>
#include <stdlib.h>

int get_format_time(millis_t millis, char *buf, int size) {
    struct tm tm_obj;
    int n, r;
    time_t seconds;

    r = 0;
    seconds = millis / SECOND_IN_MILLIS;
    localtime_r(&seconds, &tm_obj);
    n = strftime(buf, size, "%F %T", &tm_obj);
    r = n;
    buf += n;
    n = size - n;

    r += snprintf(buf, n, ".%03ld", millis % SECOND_IN_MILLIS);
    return r;
}

int my_ctime_s(millis_t millis, char *buf, int size) {
    time_t seconds;
    struct tm tm_obj;

    seconds = (time_t) (millis / SECOND_IN_MILLIS);
    return strftime(buf, size, "%a %b %d %T %Y\r\n", localtime_r(&seconds, &tm_obj));
}
