#include "thread/thread_loop.h"
#include "chapter12/chapter12.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter12_main(argc, argv);
}
