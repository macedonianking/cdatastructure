#include "process/process_util.h"

#include <unistd.h>

#include "utils/list.h"
#include "utils/log.h"
#include "utils/string_util.h"

extern char **environ;

void get_path_environ(list_head *list) {
    char **pptr, *str;
    int n;
    const char* prefix = "PATH=";

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
    string_split(str, ':', list);
}
