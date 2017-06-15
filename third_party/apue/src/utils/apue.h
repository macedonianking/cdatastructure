#ifndef APUE_SRC_APUE_H
#define APUE_SRC_APUE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>

#define APUE_FILE_MODE  0755

#define APUE_ERR_SYS() \
    err_sys("%s:%d ", __FILE__, __LINE__)

void err_sys(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_quit(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_abort();

int apue_get_fd_flags(char *buf, int buf_size, int fd);

/**
 * Clear file descriptor flags.
 */
int apue_clr_fl(int fd, int flags);
/**
 * Set file descriptor flags.
 */
int apue_set_fl(int fd, int flags);

int apue_is_dir(const char *path);
int apue_is_file(const char *path);

#endif // APUE_SRC_APUE_H
