#ifndef MAIN_UTIL_H
#define MAIN_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE	1024
#endif // MAX_BUFFER_SIZE

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))
#define MAX(a, b) ((a) < (b) ? (b) : (a))
#define MIN(a, b) ((a) > (b) ? (b) : (a))

#define CONTAINER_OF(ptr, type, m) ({ \
	(type*)((void*)(ptr) - offsetof(type, m)); \
})

static inline void fatal_error(const char *format, ...) {
	char buffer[MAX_BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, MAX_BUFFER_SIZE, format, args);
	perror(buffer);
	va_end(args);
	exit(-1);
}

#endif // MAIN_UTIL_H
