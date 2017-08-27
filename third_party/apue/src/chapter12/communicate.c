#include "chapter12/communicate.h"

#define LOG_TAG "communicate"
#include "utils/apue.h"
#include "utils/utils.h"
#include "utils/net.h"

void communicate_main(int argc, char **argv) {
    communicate_main_1(argc, argv);
}

void communicate_main_1(int argc, char **argv) {
    struct hostent item, *ptr;
    char buf[MAXLINE];
    int h_error;

    sethostent(1);
    if (!gethostbyname_r("www.ifeng.com", &item, buf, MAXLINE, &ptr, &h_error)) {
        fprintf(stdout, "%d %d\n", ptr->h_addrtype, ptr->h_length);
        while (!gethostent_r(&item, buf, MAXLINE, &ptr, &h_error) && ptr) {
            fprintf(stdout, "%d %d\n", ptr->h_addrtype, ptr->h_length);
        }
    }
    endhostent();
}
