#include "chapter9/chapter9.h"


#include <termios.h>
#include "utils/apue.h"

int chapter9_main(int argc, char **argv) {
    chapter9_3(argc, argv);
    return 0;
}

/**
 *
 */
void chapter9_3(int argc, char **argv) {
    char buf[MAXLINE];
    int n;
    sleep(3);

    for (;;) {
        if ((n = read(STDIN_FILENO, buf, MAXLINE)) == -1) {
            break;
        }
        write(STDOUT_FILENO, buf, n);
    }

    ALOGE("finish");
}
