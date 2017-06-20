#include "utils/string_util.h"

#include <stdlib.h>
#include <string.h>

char *s_strdup(const char *str) {
    char *dst = (char*) malloc(sizeof(char) *(strlen(str) + 1));
    return strcpy(dst, str);
}
