#include "thread/thread_loop.h"
#include "chapter7/chapter7.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter7_main(argc, argv);
}
