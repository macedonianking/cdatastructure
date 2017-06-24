#include "chapter14/chapter14.h"

#include "utils/apue.h"

#define MIN_FILE_SIZE   (4u << 20)

static int check_meet_file_size_requirment(int fd, size_t min_size);

int chapter14_main(int argc, char **argv) {
    chapter14_2(argc, argv);
    return 0;
}

/**
 * fcntl lock.
 */
void chapter14_2(int argc, char **argv) {
    int fd;

    if (argc != 2) {
        fprintf(stderr, "must specify an filename\n");
        exit(-1);
    }

    if ((fd = open(argv[1], O_CREAT | O_RDWR, APUE_FILE_MODE)) == -1) {
        err_sys("can't open or create file %s\n", argv[1]);
    }

    if (check_meet_file_size_requirment(fd, MIN_FILE_SIZE)) {
        close(fd);
        exit(-1);
    }
}

int check_meet_file_size_requirment(int fd, size_t min_size) {
    off_t off;

    if (lseek(fd, 0, SEEK_SET) == -1) {
        LOGE_V();
        return -1;
    }

    if ((off = lseek(fd, 0, SEEK_END)) == -1) {
        return -1;
    }

    if (off < MIN_FILE_SIZE) {
        return 0;
    }
    return -1;
}
