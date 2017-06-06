#include "chapter1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>

#include "utils/apue.h"
#include "utils/string_buffer.h"


int chapter1_main(int argc, char **argv) {
    chapter_tutorial_1_4(argc, argv);
    return 0;
}

static int list_files_rec(string_buffer_t *buf) {
    DIR *dir;
    struct dirent dir_entry, *result;
    int n, r;

    dir = opendir(buf->buf);
    if (!dir) {
        return -1;
    }

    r = 0;
    for (; !readdir_r(dir, &dir_entry, &result) && result;) {
        if (!strcmp(".", dir_entry.d_name) || !strcmp("..", dir_entry.d_name)) {
            continue;
        }
        if (dir_entry.d_name[0] == '.') {
            continue;
        }

        n = buf->size;
        string_buffer_printf(buf, "/%s", dir_entry.d_name);
        if (buf->size > PATH_MAX) {
            r = -1;
            goto out;
        }
        fprintf(stdout, "%s\n", buf->buf);
        list_files_rec(buf);
        buf->buf[n] = '\0';
        buf->size = n;
    }

out:
    closedir(dir);
    return r;
}

void chapter_tutorial_1_4(int argc, char **argv) {
    string_buffer_t buf;
    int r;
    if (argc  < 2) {
        err_sys("You need provide a path.");
    }

    string_buffer_init(&buf);
    string_buffer_append(&buf, argv[1]);
    r = list_files_rec(&buf);
    string_buffer_free(&buf);

    if (r) {
        err_quit("list_files_rec fail.");
    }
}
