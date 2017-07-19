#include "thread/thread_loop.h"
#include "chapter5/chapter5.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter5_main(argc, argv);
}
