#ifndef APUE_SRC_APUE_H
#define APUE_SRC_APUE_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <poll.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils/log.h"
#include "utils/time_util.h"

#include "utils/list.h"
#include "utils/log.h"
#include "utils/string_buffer.h"
#include "utils/string_util.h"
#include "utils/utils.h"

#define APUE_FILE_MODE  0755

#define APUE_ERR_SYS() \
    err_sys("%s:%d ", __FILE__, __LINE__)

#define APUE_SET_FD_FL 0
#define APUE_ADD_FD_FL 1
#define APUE_DEL_FD_FL 2

void err_sys(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_quit(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void err_abort();

int apue_get_fd_flags(char *buf, int buf_size, int fd);
int apue_dup_stderr(const char* err_file);

/**
 * Clear file descriptor flags.
 */
int apue_clr_fl(int fd, int flags);
/**
 * Set file descriptor flags.
 */
int apue_set_fl(int fd, int flags);

/**
 * Update file descriptor flags.
 */
int apue_set_fd_flags(int fd, int how, int in_flags, int *out_flags);

int apue_is_dir(const char *path);
int apue_is_file(const char *path);

void apue_default_signal_handler(int signo);
void apue_default_sigaction(int signo, siginfo_t *info, void *context);

/**
 * @param in_fd:    read from in file descriptor.
 * @param out_fd:   write to out file descriptor.
 */
int apue_fd_copy(int in_fd, int out_fd);

int apue_read_file_in_size(const char *name, int out_fd, int min_size);

/**
 *
 */
int apue_handle_file_in_size(const char *name, int min_size, 
                             int (*handler)(void *, char *, int), 
                             void *data);

ssize_t readn(int fd, void *buf, size_t size);
ssize_t writen(int fd, void *buf, size_t size);
ssize_t dgetline(int fd, char **lineptr, size_t *n);

int wait_rd(int fd, int timeout);
int wait_wr(int fd, int timeout);

int ensure_memory_size(char **mem, size_t *mem_size, size_t count);

#endif // APUE_SRC_APUE_H
