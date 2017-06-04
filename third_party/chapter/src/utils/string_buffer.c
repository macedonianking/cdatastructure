#include "string_buffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "log.h"

#define DEFAULT_STRNG_BUFFER_CAPACITY   1024

static void extend_string_buffer_size(struct string_buffer_t *ptr, int n);

void string_buffer_init(struct string_buffer_t *ptr) {
    ptr->buf = (char*) malloc(sizeof(char) * DEFAULT_STRNG_BUFFER_CAPACITY);
    ptr->buf[0] = '\0';
    ptr->capacity = 1024;
    ptr->size = 0;
}

void string_buffer_release(struct string_buffer_t *ptr) {
    free(ptr->buf);
    ptr->size = ptr->capacity = 0;
}

/**
* 执行字符串的格式化
*/
int string_buffer_printf(struct string_buffer_t *ptr,
                         const char *format, ...) {
    int n, capacity;
    va_list args;

    capacity = ptr->capacity - ptr->size;
    if (capacity < 125) {
        extend_string_buffer_size(ptr, 2 * ptr->capacity);
        capacity = ptr->capacity - ptr->size;
    }

    va_start(args, format);
    do {
        n = vsnprintf(ptr->buf + ptr->size, capacity, format, args);
        if (n + 1 == capacity) {
            n = 0;
            extend_string_buffer_size(ptr, 2 * ptr->capacity);
            capacity = ptr->capacity - ptr->size;
        }
    } while (!n);
    va_end(args);

    ptr->size += n;
    return n;
}

void extend_string_buffer_size(struct string_buffer_t *ptr, int n) {
    DCHECK(n > ptr->capacity && n > 0);
    ptr->buf = realloc(ptr->buf, n);
    DCHECK(ptr->buf);
}

int string_buffer_append(struct string_buffer_t *ptr,
        const char *str) {
    return string_buffer_printf(ptr, "%s", str);
}
