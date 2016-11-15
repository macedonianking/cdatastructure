#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

void fatal_error(const char *format, ...)
{
	char buffer[MAX_BUFFER_SIZE];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, MAX_BUFFER_SIZE, format, args);
	perror(buffer);
	va_end(args);
	exit(-1);
}
