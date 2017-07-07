#include "thread/thread_loop.h"
#include "chapter16/chapter16.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter16_main(argc, argv);
}
