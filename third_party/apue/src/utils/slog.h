#ifndef APUE_SRC_UTILS_SLOG_H
#define APUE_SRC_UTILS_SLOG_H

#include <syslog.h>

#define SLOGE(format, ...) syslog(LOG_ERR | LOG_USER, format, ##__VA_ARGS__)
#define SLOGW(format, ...) syslog(LOG_WARNING | LOG_USER, format, ##__VA_ARGS__)
#define SLOGD(format, ...) syslog(LOG_DEBUG | LOG_USER, format, ##__VA_ARGS__)
#define SLOGI(format, ...) syslog(LOG_INFO | LOG_USER, format, ##__VA_ARGS__)

#endif 
