#ifndef THIRD_PARTY_CHAPTER_SRC_UTILS_ARG_PARSE_H
#define THIRD_PARTY_CHAPTER_SRC_UTILS_ARG_PARSE_H

#include <stdio.h>
#include <limits.h>

#include "list.h"

#define DEFAULT_INT_NONE    INT_MAX

struct arg_parser;
struct arg_parser_result_t;

enum arg_type_t {
    arg_type_none,          // 没有指定类型
    arg_type_int_switch,    // 参数开关
    arg_type_string,        // 单个的字符串选项
    arg_type_list,          // 多个字符串的列表
    arg_type_positions,     // 多个的位置参数
};

struct v_string_arg_t {
    struct list_head    node;
    char                *str;
};

struct arg_desc_t {
    struct list_head node;
    char            *long_name;
    char            *short_name;
    char            *option_name;
    char            *desc;
    char            *metavar;
    int             type;
    int             default_int;
    int             is_required;
    char            *default_string;
};

// 参数解析器
struct arg_parser *open_arg_parser();
void close_arg_parser(struct arg_parser *parser);
int arg_parser_add_argument(struct arg_parser *parser, struct arg_desc_t *desc);
void arg_parser_print_help(FILE *fp, struct arg_parser *parser, const char *exec_name);

// 解析参数
struct arg_parser_result_t *arg_parser_parse_args(char **str, int start, int end);
// 销毁解析参数的结果
void close_arg_parser_result(struct arg_parser_result_t *results);

#endif
