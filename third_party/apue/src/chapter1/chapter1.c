#include "chapter1.h"

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "utils/apue.h"
#include "utils/list.h"
#include "utils/string_buffer.h"
#include "utils/string_ext.h"
#include "utils/utils.h"


int chapter1_main(int argc, char **argv) {
    chapter_tutorial_1_6(argc, argv);
    return 0;
}

static int list_files_rec(string_buffer_t *buf) {
    DIR *dir;
    struct dirent dir_entry, *result;
    int n, old_size, r;

    dir = opendir(buf->buf);
    if (!dir) {
        return 0;
    }

    r = 0;
    old_size = buf->size;
    if (buf->size > 0 && buf->buf[buf->size-1] != '/') {
        string_buffer_append(buf, "/");
    }
    for (; !readdir_r(dir, &dir_entry, &result) && result;) {
        if (!strcmp(".", dir_entry.d_name) || !strcmp("..", dir_entry.d_name)) {
            continue;
        }
        if (dir_entry.d_name[0] == '.') {
            continue;
        }

        n = buf->size;
        string_buffer_printf(buf, "%s", dir_entry.d_name);
        if (buf->size > PATH_MAX) {
            r = -1;
            goto out;
        }
        fprintf(stdout, "%s\n", buf->buf);
        list_files_rec(buf);
        buf->buf[n] = '\0';
        buf->size = n;
    }
    buf->buf[old_size] = '\0';

out:
    closedir(dir);
    return r;
}

void chapter_tutorial_1_4(int argc, char **argv) {
    string_buffer_t buf;
    int r;
    if (argc  < 2) {
        err_sys("You need provide a path.");
        return;
    }

    string_buffer_init(&buf);
    string_buffer_append(&buf, argv[1]);
    r = list_files_rec(&buf);
    string_buffer_free(&buf);

    if (r) {
        err_quit("list_files_rec fail.");
    }
}

void chapter_tutorial_1_5(int argc, char **argv) {
    char buffer[NBUF_SIZE];
    int n;

    while((n = read(STDIN_FILENO, buffer, NBUF_SIZE)) > 0) {
        if((n = write(STDOUT_FILENO, buffer, n)) != n) {
            n = -1;
            break;
        }
    }

    if (n < 0) {
        err_sys("chapter_tutorial_1_5");
    }
}

static void parse_command_options(const char *str, char ***out, size_t *len) {
    const char *end;
    char **out_p;
    DEFINE_LIST_HEAD(list);
    struct string_node_t *ptr;
    int n, i;

    n = 0;
    while (*str != '\0') {
        str += strspn(str, "\n\t ");
        if (*str == '\0') {
            break;
        }

        end = str + strcspn(str, "\n\t ");
        ptr = (struct string_node_t *) malloc(sizeof(*ptr));
        INIT_LIST_HEAD(&ptr->node);
        ptr->str = (char *) malloc(end - str + 1);
        strncpy(ptr->str, str, end - str);
        ptr->str[end-str] = '\0';
        list_add_tail(&ptr->node, &list);
        ++n;
        str = end;
    }

    *len = n;
    if (!n) {
        return;
    }

    out_p = (char**) malloc((n+1) * sizeof(char*));
    i = 0;
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &list, node) {
        out_p[i++] = ptr->str;
        ptr->str = NULL;
        list_del(&ptr->node);
        free(ptr);
    }
    out_p[i] = NULL;
    *out = out_p;
}

void chapter_tutorial_1_6(int argc, char **argv) {
    char *line, **args;
    size_t args_n;
    size_t nline;
    int n;
    pid_t child;

    line = NULL;
    nline = 0;
    fprintf(stdout, "%%:");
    while ((n = getline(&line, &nline, stdin)) > 0) {
        if (n > 0 && line[n-1] == '\n') {
            line[n-1] = '\0';
            n -= 1;
        }

        if((child = fork()) < 0) {
            err_sys("fork");
            break;
        } else if (!child) {
            args_n = 0;
            args = NULL;
            parse_command_options(line, &args, &args_n);
            if (args_n > 0) {
                execvp(args[0], args);
                err_quit("");
            }
            break;
        }

        waitpid(child, NULL, 0);
        fprintf(stdout, "%%:");
    }

    free(line);
    line = NULL;
    fputc('\n', stdout);
}
