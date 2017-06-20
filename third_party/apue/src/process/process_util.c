#include "process/process_util.h"

#include <unistd.h>

#include "utils/list.h"
#include "utils/log.h"
#include "utils/string_util.h"

extern char **environ;

void get_path_environ(list_head *list) {
    char **pptr, *str, *next;
    int n;
    const char* prefix = "PATH=";
    string_node_t *str_node;

    n = strlen(prefix);
    pptr = environ;
    while (*pptr) {
        if (!strncmp(*pptr, prefix, n)) {
            break;
        }
        ++pptr;
    }
    if (!*pptr) {
        return;
    }
    str = *pptr + n;
    while (*str != '\0') {
        if (*str == ':') {
            str_node = alloc_string_node("");
            list_add_tail(&str_node->node, list);
            str++;
            continue;
        }

        next = strchr(str, ':');
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
