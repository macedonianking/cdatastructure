#include "thread/thread_loop.h"
#include "chapter15/chapter15.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter15_main(argc, argv);
}
