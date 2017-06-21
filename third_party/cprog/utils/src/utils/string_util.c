#include "utils/string_util.h"

#include <stdlib.h>
#include <string.h>

#include "utils/log.h"

char *s_strdup(const char *str) {
    char *dst = (char*) malloc(sizeof(char) *(strlen(str) + 1));
    return strcpy(dst, str);
}

void string_split(const char *str, char sep, list_head *list) {
    string_node_t *str_node;
    char *next;
    int n;

    while (*str != '\0') {
        if (*str == sep) {
            str_node = alloc_string_node("");
            list_add_tail(&str_node->node, list);
            str++;
            continue;
        }

        next = strchr(str, sep);
        if (next) {
            n = next - str;
            DCHECK(n);
            str_node = alloc_string_node(NULL);
            str_node->str = (char*) malloc(sizeof(char) * (n + 1));
            strncpy(str_node->str, str, n);
            str_node->str[n] = '\0';
            list_add_tail(&str_node->node, list);
            str = next + 1;
        } else {
            str_node = alloc_string_node(str);
            list_add_tail(&str_node->node, list);
            break;
        }
    }
}
