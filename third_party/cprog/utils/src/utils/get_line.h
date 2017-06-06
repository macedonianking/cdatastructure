#ifndef CHAPTER_SRC_CHAPTER1_GET_LINE_H
#define CHAPTER_SRC_CHAPTER1_GET_LINE_H

#include <stdio.h>

struct get_line_t;

/**
 * 打开获取行的对象
 */
struct get_line_t *open_getline();

/**
 * 获取一行
 */
int read_getline(struct get_line_t *get, FILE *fp, char **out, int *nout);

/**
 * 关闭获取行的对象
 */
void close_getline(struct get_line_t*);

#endif // CHAPTER_SRC_CHAPTER1_GET_LINE_H
