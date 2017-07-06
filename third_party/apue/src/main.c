#include "thread/thread_loop.h"
#include "chapter14/chapter14.h"

int main(int argc, char **argv) {
    ThreadLoopInitialize();
    return chapter14_main(argc, argv);
}
