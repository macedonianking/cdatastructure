#include "chapter13/daemon_process.h"

#include "utils/slog.h"
#include "utils/apue.h"

void daemon_process_main(int argc, char **argv) {
    syslog(LOG_DEBUG|LOG_USER, "Hello world %m");
}
