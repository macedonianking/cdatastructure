#include "chapter3/sock_struct.h"

#define LOG_TAG "sock_struct"

#include "utils/apue.h"
#include "utils/net.h"
#include "utils/log.h"

void chapter3_sock_struct_main(int argc, char **argv) {
    chapter3_sock_struct_main_3(argc, argv);
}

void chapter3_sock_struct_main_1(int argc, char **argv) {
    struct sockaddr_in addr;
    int result;

    bzero(&addr, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    result = inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    ALOGE_ALWAYSE_FATAL_IF(result != 1,
        "chapter3_sock_struct_main_1 inet_pton FATAL(%s)",
        strerror(errno));
}

void chapter3_sock_struct_main_2(int argc, char** argv) {
    char buf[BUFSIZ];
    struct hostent ent, *result_ent;
    int host_error;
    int r;
    char ip_addr[INET6_ADDRSTRLEN];
    char **addr_list;
    millis_t millis;

    millis = current_time_millis();
    r = gethostbyname_r("www.baidu.com", &ent, buf, BUFSIZ,
            &result_ent, &host_error);
    millis = current_time_millis() - millis;
    ALOGD("delta=%ld", millis);
    ALOGE_ALWAYSE_FATAL_IF(r, "gethostbyname_r FATAL(%s)",
        strerror(errno));
    if (result_ent != &ent) {
        return;
    }
    fprintf(stdout, "%s\n", ent.h_name);
    addr_list = ent.h_addr_list;
    while (*addr_list) {
        if (inet_ntop(AF_INET, *addr_list, ip_addr, INET6_ADDRSTRLEN) == ip_addr) {
            fprintf(stdout, "%s\n", ip_addr);
        }
        ++addr_list;
    }
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
