#include "thread/thread_loop.h"
#include "chapter6/chapter6.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter6_main(argc, argv);
}
