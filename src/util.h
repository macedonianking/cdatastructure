#ifndef MAIN_UTIL_H
#define MAIN_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
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

void shuffle(int *data, int n);
int *linear_sequence(int start, int step, int n);

#endif // MAIN_UTIL_H
