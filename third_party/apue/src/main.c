#include "thread/thread_loop.h"
#include "chapter3/chapter3.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter3_main(argc, argv);
}
