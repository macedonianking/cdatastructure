#include "thread/thread_loop.h"
#include "chapter11/chapter11.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter11_main(argc, argv);
}
