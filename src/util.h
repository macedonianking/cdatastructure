#ifndef MAIN_UTIL_H
#define MAIN_UTIL_H

#include <stddef.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE	1024
#endif // MAX_BUFFER_SIZE

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

#define CONTAINER_OF(ptr, type, m) ({ \
	(type*)((void*)(ptr) - offsetof(type, m)); \
})

#endif // MAIN_UTIL_H
