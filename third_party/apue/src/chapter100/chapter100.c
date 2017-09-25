#include "chapter100/chapter100.h"

#define LOG_TAG "chapter100"
#include "utils/log.h"
#include "utils/apue.h"
#include "chapter11/dns_resolve.h"

int chapter100_main(int argc, char **argv) {
    dns_resolve_main(argc, argv);
    return 0;
}
