#include "thread/thread_loop.h"
#include "chapter10/chapter10.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter10_main(argc, argv);
}
