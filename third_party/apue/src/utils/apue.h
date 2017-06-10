#ifndef APUE_SRC_APUE_H
#define APUE_SRC_APUE_H

#include <unistd.h>
#include <fcntl.h>

#define APUE_FILE_MODE  0755

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

#endif // APUE_SRC_APUE_H
