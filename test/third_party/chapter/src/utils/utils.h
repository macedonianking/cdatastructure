#ifndef THIRDPARTY_CHAPTER_SRC_UTILS_UTILS_H
#define THIRDPARTY_CHAPTER_SRC_UTILS_UTILS_H

#include <stddef.h>

/*
* 缓冲区的大小
*/
#ifndef NBUF_SIZE
#define NBUF_SIZE   1024
#endif

#define NARRAY(a)   (sizeof(a) / sizeof((a)[0]))

#define CONTAINER_OF(ptr, type, m) ({ \
    (type*)((void*)(ptr) - offsetof(type, m)); \
})

#endif // THIRDPARTY_CHAPTER_SRC_UTILS_UTILS_H
