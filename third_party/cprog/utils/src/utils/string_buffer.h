#ifndef CHAPTER_SRC_UTILS_STRING_BUFFER_H
#define CHAPTER_SRC_UTILS_STRING_BUFFER_H

typedef struct string_buffer_t {
    char    *buf;
    int     size;
    int     capacity;
} string_buffer_t;

void    string_buffer_init(string_buffer_t *ptr);
void    string_buffer_free(string_buffer_t *ptr);
int     string_buffer_printf(string_buffer_t *ptr, const char *format, ...) __attribute__((format(printf, 2, 3)));

// 添加字符串到后缀
int     string_buffer_append(string_buffer_t *ptr, const char *str);
int     string_buffer_assign(string_buffer_t *ptr, const char *str);

#endif
