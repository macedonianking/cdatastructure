#include "apue.h"

#include <stdio.h>
#include <stdlib.h>

void err_sys(const char *msg) {
    perror(msg);
}

void err_quit(const char *msg) {
    perror(msg);
    exit(-1);
}
