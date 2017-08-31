#include "chapter3/sock_struct.h"

#define LOG_TAG "sock_struct"

#include "utils/apue.h"
#include "utils/net.h"
#include "utils/log.h"

void chapter3_sock_struct_main(int argc, char **argv) {
    chapter3_sock_struct_main_2(argc, argv);
}

/**
 * 填充地址
 */
void chapter3_sock_struct_main_1(int argc, char **argv) {
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) == 1) {
        ALOGE("inet_pton success");
    }
}

void chapter3_sock_struct_main_2(int argc, char** argv) {
    struct sockaddr_in addr;

    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    ALOGE("%s", sock_ntop((SA*)&addr, sizeof(addr)));
}

void chapter3_sock_struct_main_3(int argc, char **argv) {
    int r;
    struct sockaddr_in addr;
    char *lineptr;
    size_t lineptr_size;
    int fd;

    r = resolve_host("www.baidu.com", "http", &addr);
    ALOGE_ALWAYSE_FATAL_IF(r, "resolve_host  FATAL");

    fprintf(stdout, "result=%s\n", sock_ntop((struct sockaddr*)&addr, sizeof(struct sockaddr_in)));

    lineptr = NULL;
    lineptr_size = 0;

    if ((fd = open("/etc/passwd", O_RDONLY)) == -1) {
        return;
    }
    while (dgetline(fd, &lineptr, &lineptr_size) > 0) {
        fprintf(stdout, "%s", lineptr);
    }
    close(fd);
    free(lineptr);
    lineptr = NULL;
}
