#ifndef THIRDPARTY_CHAPTER_SRC_UTILS_STRING_EXT_H
#define THIRDPARTY_CHAPTER_SRC_UTILS_STRING_EXT_H

#include <string.h>

char *s_strdup(const char *str);

static inline int s_is_empty(const char *str) {
    return !str || *str == '\0';
}

static inline int s_startwith(const char *str, const char *pattern) {
    if (!str || !pattern) {
        return 0;
    }

    while (*pattern != '\0') {
        if (*str != *pattern) {
            break;
        }
        ++str;
        ++pattern;
    }
    return *pattern == '\0';
}

#endif // THIRDPARTY_CHAPTER_SRC_UTILS_STRING_EXT_H
