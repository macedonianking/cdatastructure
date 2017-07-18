#include "thread/thread_loop.h"
#include "chapter100/chapter100.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter100_main(argc, argv);
}
