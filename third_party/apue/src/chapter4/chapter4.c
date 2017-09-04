#include "chapter4.h"

#include "utils/apue.h"
#include "utils/utils.h"
#include "utils/string_buffer.h"
#include "chapter4/tcp_socket_elementary.h"

static inline const char *get_file_type_description(int st_mode) {
    if (S_ISREG(st_mode)) {
        return "regular file";
    } else if (S_ISDIR(st_mode)) {
        return "directory";
    } else if (S_ISCHR(st_mode)) {
        return "character special";
    } else if (S_ISBLK(st_mode)) {
        return "block special";
    } else if (S_ISLNK(st_mode)) {
        return "symbolic link";
    } else if (S_ISFIFO(st_mode)) {
        return "fifo";
    } else if (S_ISSOCK(st_mode)) {
        return "socket";
    } else {
        return "unknown type";
    }
}

static void dump_standard_file_description();
static void test_lstat();
static void test_fstatat();
static void test_setuid_setgid();

static inline
const char *get_fd_type_description(int fd) {
    struct stat stat_obj;

    if (fstat(fd, &stat_obj)) {
        err_sys("fstat %d", fd);
    }
    return get_file_type_description(stat_obj.st_mode);
}

int chapter4_main(int argc, char **argv) {
    return 0;
}

void chapter4_3(int argc, char **argv) {
    struct stat stat_obj;

    for (int i = 1; i < argc; ++i) {
        if (!stat(argv[1], &stat_obj)) {
            fprintf(stdout, "%s: %s\n", argv[i], get_file_type_description(stat_obj.st_mode));
        }
    }

    dump_standard_file_description();
    test_lstat();
    test_fstatat();
    test_setuid_setgid(&stat_obj);
}

#define CHECK_ST_MODE(r, is_first, mode, mode_name) \
    do {    \
        if ((mode) & mode_name) {   \
            if (is_first) { \
                is_first = 0;   \
                string_buffer_printf((r), "%s", #mode_name);    \
            } else {    \
                string_buffer_printf((r), " | %s", #mode_name); \
            }   \
        }   \
    } while (0)

void chapter4_5(int argc, char **argv) {
    struct stat stat_obj;
    int is_first;
    string_buffer_t str_buf;

    if (argc < 2) {
        return;
    }

    if (stat(argv[1], &stat_obj)) {
        err_sys("can't stat '%s'", argv[1]);
    }

    string_buffer_init(&str_buf);
    is_first = 1;
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IRUSR);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IWUSR);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IXUSR);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IRGRP);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IWGRP);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IXGRP);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IROTH);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IWOTH);
    CHECK_ST_MODE(&str_buf, is_first, stat_obj.st_mode, S_IXOTH);

    printf("%s st_mode = %s\n", argv[1], str_buf.buf);
    string_buffer_free(&str_buf);
}

void dump_standard_file_description() {
    fprintf(stdout, "%s: %s\n", "STDIN_FILENO", get_fd_type_description(STDIN_FILENO));
    fprintf(stdout, "%s: %s\n", "STDOUT_FILENO", get_fd_type_description(STDOUT_FILENO));
    fprintf(stdout, "%s: %s\n", "STDERR_FILENO", get_fd_type_description(STDERR_FILENO));
}

void test_lstat() {
    struct stat a_stat_obj, b_stat_obj;

    if (lstat("a.txt", &a_stat_obj) || lstat("b.txt", &b_stat_obj)) {
        return;
    }

    fprintf(stdout, "a.txt:%s\n", get_file_type_description(a_stat_obj.st_mode));
    fprintf(stdout, "b.txt:%s\n", get_file_type_description(b_stat_obj.st_mode));
}

void test_fstatat() {
    struct stat stat_obj;

    if (fstatat(AT_FDCWD, "b.txt", &stat_obj, 0)) {
        return;
    }
    fprintf(stdout, "b.txt:%s\n", get_file_type_description(stat_obj.st_mode));
}

void test_setuid_setgid(struct stat *buf) {
    if (buf->st_mode & S_ISUID) {
        fprintf(stdout, "S_ISUID\n");
    }
    if (buf->st_mode & S_ISGID) {
        fprintf(stdout, "S_ISGID\n");
    }
}

static void check_create_file_owner(int argc, char **argv) {
    int fd;
    struct stat stat_obj;

    if (argc < 2) {
        return;
    }

    if((fd = open(argv[1], O_RDWR | O_CREAT, APUE_FILE_MODE)) < 0) {
        err_sys("check_create_file_owner");
    }
    if (!fstat(fd, &stat_obj)) {
        fprintf(stdout, "file.st_uid=%d, file.st_gid=%d\n",
            stat_obj.st_uid, stat_obj.st_gid);
    }
    close(fd);
}

void chapter4_6(int argc, char **argv) {
    fprintf(stdout, "uid=%d, gid=%d, euid=%d, egid=%d\n",
        getuid(), getgid(), geteuid(), getegid());
    check_create_file_owner(argc, argv);
}

void chapter4_7(int argc, char **argv) {
    if (argc < 2)
        return;
}

void chapter4_8(int argc, char **argv) {
    mode_t mask_value;
    int fd;

    mask_value = umask(0);
    umask(mask_value);
    fprintf(stdout, "umask=0%o\n", mask_value);

    if ((fd = creat("bar", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
        err_sys("creat");
    }
    close(fd);
    umask(S_IRGRP | S_IWGRP);
    if ((fd = creat("foo", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
        err_sys("creat");
    }
    close(fd);
}

void chapter4_9(int argc, char **argv) {
    struct stat stat_obj;
    mode_t mode;

    if (argc < 2)
        return;

    if (stat(argv[1], &stat_obj)) {
        APUE_ERR_SYS();
    }

    mode = stat_obj.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);
    mode |= S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
    if(chmod(argv[1], mode)) {
        APUE_ERR_SYS();
    }
}
