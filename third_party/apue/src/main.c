#include "thread/thread_loop.h"
#include "chapter1/chapter1.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter1_main(argc, argv);
}
