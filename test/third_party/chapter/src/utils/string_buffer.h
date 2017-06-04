#ifndef CHAPTER_SRC_UTILS_STRING_BUFFER_H
#define CHAPTER_SRC_UTILS_STRING_BUFFER_H

struct string_buffer_t {
    char    *buf;
    int     size;
    int     capacity;
};

void string_buffer_init(struct string_buffer_t *ptr);
void string_buffer_release(struct string_buffer_t *ptr);
int string_buffer_printf(struct string_buffer_t *ptr, 
        const char *format, ...);

// 添加字符串到后缀
int string_buffer_append(struct string_buffer_t *ptr,
        const char *str);

#endif
