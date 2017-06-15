#include "chapter3.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils/apue.h"
#include "utils/string_buffer.h"

int chapter3_main(int argc, char **argv) {
    chapter_exercise_3_6(argc, argv);
    return 0;
}

void chapter3_2(int argc, char **argv) {
    int fd;
    int n;
    char buffer[BUFSIZ];
    string_buffer_t str_buf;


    if (argc < 2) {
        err_quit("argument error");
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err_sys("can't open file '%s'", argv[1]);
    }

    string_buffer_init(&str_buf);
    if ((n = read(fd, buffer, BUFSIZ-1)) > 0) {
        buffer[n] = '\0';
        string_buffer_append(&str_buf, buffer);
    }

    if (n < 0) {
        perror("");
        goto out;
    }

    fputs(str_buf.buf, stdout);
    fputc('\n', stdout);

out:
    string_buffer_free(&str_buf);
    close(fd);
}

void chapter3_11(int argc, char **argv) {
    int fd;
    int n;
    const char *str;
    off_t offset;

    if (argc < 2) {
        err_quit("Must specify a file in command options.");
    }

    if ((fd = open(argv[1], O_RDWR | O_CREAT)) < 0) {
        err_sys("can't open file '%s'", argv[1]);
    }

    if ((offset = lseek(fd, 0, SEEK_SET)) < 0) {
        err_sys("can't seek file '%s'", argv[1]);
    }

    fprintf(stdout, "seek initial position %ld\n", (long) offset);
    str = "Hello world";
    n = strlen(str);
    if (pwrite(fd, str, strlen(str), offset) != n) {
        err_sys("pwrite error");
    }
    if ((offset = lseek(fd, 0, SEEK_CUR)) < 0) {
        err_sys("can't seek");
    }
    fprintf(stdout, "seek position after pwrite is %ld\n", (long) offset);
    close(fd);
}

void chapter3_13(int argc, char **argv) {
    int fd;
    int r;
    char buf[BUFSIZ];

    if (argc < 2) {
        return;
    }

    if ((fd = open(argv[1], O_RDWR | O_CREAT | O_SYNC), APUE_FILE_MODE) < 0) {
        err_sys("can't open file '%s'", argv[1]);
    }
    r = fcntl(fd, F_GETFL, 0);
    if (r < 0) {
        err_sys("fcntl %s:%d", __FILE__, __LINE__);
    }

    apue_get_fd_flags(buf, BUFSIZ, fd);
    fprintf(stdout, "'%s' open flags %s\n", argv[1], buf);
    apue_get_fd_flags(buf, BUFSIZ, STDOUT_FILENO);
    fprintf(stdout, "stdout open flags %s\n", buf);

    close(fd);
}

static int self_dup2(int fd, int fd2) {
    char buf[BUFSIZ];
    int fd_flags;
    int fd2_flags;

    if ((fd_flags = fcntl(fd, F_GETFL, 0)) < 0) {
        return -1;
    }

    if((fd2 != fd)) {
        if ((fd2_flags = fcntl(fd, F_GETFL, 0)) >= 0) {
            close(fd2);
        }
    }

    snprintf(buf, BUFSIZ, "/dev/fd/%d", fd);
    if ((fd2 = open(buf, fd_flags, APUE_FILE_MODE)) < 0) {
        return -1;
    }
    return fd2;
}

void chapter_exercise_3_2(int argc, char **argv) {
    int fd2;

    fd2 = self_dup2(STDOUT_FILENO, 3);
    if (fd2 < 0) {
        err_sys("can't dump STDOUT_FILENO");
    }

    fprintf(stdout, "self_dup2 success:%d\n", fd2);
    close(fd2);
}

void chapter_exercise_3_6(int argc, char **argv) {
    int fd;
    int n;
    off_t offset;
    const char *str;

    if (argc < 2) {
        err_quit("command options error");
    }

    if ((fd = open(argv[1], O_RDWR | O_APPEND| O_CREAT, 
            APUE_FILE_MODE)) < 0) {
        err_sys("can't open file '%s'", argv[1]);
    }

    str = "Hello world\n";
    n = strlen(str);
    if (write(fd, str, n) != n) {
        err_sys("write");
    }

    offset = lseek(fd, 0, SEEK_CUR);
    fprintf(stdout, "lseek cur %d\n", (int)offset);

    offset = lseek(fd, 0, SEEK_SET);
    fprintf(stdout, "lseek begin %d\n", (int)offset);
}
