#ifndef THIRDPARTY_CHAPTER_SRC_UTILS_STRING_EXT_H
#define THIRDPARTY_CHAPTER_SRC_UTILS_STRING_EXT_H

#include <string.h>
#include <stdlib.h>

#include "utils/list.h"

typedef struct string_node_t {
    struct list_head    node;
    char                *str;
} string_node_t;

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

static inline string_node_t *alloc_string_node(const char *str) {
    string_node_t *ptr;

    ptr = (string_node_t*) malloc(sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->node);
    ptr->str = s_strdup(str);

    return ptr;
}

static inline void free_string_node(string_node_t *node) {
    free(node->str);
    free(node);
}

#endif // THIRDPARTY_CHAPTER_SRC_UTILS_STRING_EXT_H
