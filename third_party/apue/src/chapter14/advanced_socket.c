#include "advanced_socket.h"

#include <string.h>
#include <sys/uio.h>

#define LOG_TAG "advanced_socket"
#include "utils/apue.h"
#include "utils/net.h"
#include "utils/utils.h"

void advanced_socket_main(int argc, char **argv) {
    advanced_socket_main_1(argc, argv);
}

void send_message(int fd) {
    struct msghdr msg, *ptr;
    struct sockaddr_in addr;
    struct iovec vec;
    char buf[MAXLINE];
    struct cmsghdr control;

    vec.iov_base = buf;
    vec.iov_len = MAXLINE;
    ptr = &msg;
    memset(ptr, 0, sizeof(*ptr));
    ptr->msg_name = &addr;
    ptr->msg_namelen = sizeof(addr);
    ptr->msg_iov = &vec;
    ptr->msg_iovlen = 1;
    memset(&control, 0, sizeof(control));
    ptr->msg_control = &control;
    ptr->msg_controllen = sizeof(control);

    if (resolve_host("www.baidu.com", "http", &addr)) {
        return;
    }
}

void advanced_socket_main_1(int argc, char **argv) {

}
