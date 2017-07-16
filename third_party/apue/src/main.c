#include "thread/thread_loop.h"
#include "chapter4/chapter4.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter4_main(argc, argv);
}
