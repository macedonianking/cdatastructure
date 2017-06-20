#include "arg_parse.h"

#include <stdlib.h>
#include <memory.h>

#include "utils/utils.h"
#include "utils/log.h"
#include "utils/math_help.h"
#include "utils/string_util.h"
#include "utils/string_buffer.h"

struct arg_parser_t {
    struct list_head arg_list;
};

struct arg_parser_options_t {
    char *exec_name;
    struct list_head list;
};

struct arg_parser_option_node_t {
    struct list_head node;
    struct arg_desc_t *desc;
    struct arg_option_t option;
};

#define CLONE_STRING(str) ((str) ? s_strdup(str) : (str))

static struct arg_desc_t *arg_parser_find_arg_desc(
        struct arg_parser_t *parser,
        const char *name);
static struct arg_desc_t *arg_parser_find_position_desc(
        struct arg_parser_t *parser);

static struct arg_parser_option_node_t *alloc_arg_parser_option_node();
static void free_arg_parser_option_node(struct arg_parser_option_node_t *ptr);
static struct arg_parser_option_node_t *find_arg_parser_option_node(
        struct arg_parser_options_t *options,
        const char *name);

struct arg_parser_t *open_arg_parser() {
    struct arg_parser_t *parser = (struct arg_parser_t*) malloc(sizeof(struct arg_parser_t));
    INIT_LIST_HEAD(&parser->arg_list);
    return parser;
}

struct arg_desc_t *arg_parser_find_arg_desc(
        struct arg_parser_t *parser,
        const char *name) {
    struct arg_desc_t *ptr;

    if (s_is_empty(name)) {
        return 0;
    }

    LIST_FOR_EACH_ENTRY(ptr, &parser->arg_list, node) {
        if ((ptr->long_name && !strcmp(ptr->long_name, name))
            || (ptr->short_name && !strcmp(ptr->short_name, name))
            || (ptr->option_name && !strcmp(ptr->option_name, name))) {
            return ptr;
        }
    }
    return NULL;
}

struct arg_desc_t *arg_parser_find_position_desc(
        struct arg_parser_t *parser) {
    struct arg_desc_t *ptr;

    LIST_FOR_EACH_ENTRY(ptr, &parser->arg_list, node) {
        if (ptr->type == arg_type_positions) {
            return ptr;
        }
    }
    return NULL;
}

static inline void normalize_option_name(char *str) {
    while (*str != '\0') {
        if (*str == '-') {
            *str = '_';
        }
        ++str;
    }
}

static inline int check_arg_name(const char *name, const char *prefix) {
    return !s_is_empty(name) && s_startwith(name, prefix) && name[strlen(prefix)] != '\0';
}

static inline int check_desc_arg_name(struct arg_desc_t *ptr) {
    return check_arg_name(ptr->long_name, "--") || check_arg_name(ptr->short_name, "-");
}

/**
 * 检查参数的完整性
 */
static int check_arg_desc(struct arg_parser_t *parser, struct arg_desc_t *ptr) {
    DCHECK(ptr);
    switch(ptr->type) {
        case arg_type_int_switch: {
            break;
        }
        case arg_type_string:
        case arg_type_list:
        case arg_type_positions: {
            if (s_is_empty(ptr->metavar)) {
                return 0;
            }
            break;
        }
        default: {
            return 0;
        }
    }

    if (ptr->type != arg_type_positions) {
        if (!check_desc_arg_name(ptr)) {
            return 0;
        }
    } else {
        if (check_desc_arg_name(ptr)) {
            return 0;
        }
        if (arg_parser_find_position_desc(parser)) {
            return 0;
        }
    }

    if (s_is_empty(ptr->option_name)) {
        free(ptr->option_name);
        ptr->option_name = NULL;

        if (check_arg_name(ptr->long_name, "--")) {
            ptr->option_name = CLONE_STRING(ptr->long_name+2);
        }
        if (!ptr->option_name && check_arg_name(ptr->short_name, "-")) {
            ptr->option_name = CLONE_STRING(ptr->short_name+1);
        }
        if (ptr->option_name) {
            for (char *q = ptr->option_name; *q != '\0'; ++q) {
                if (*q == '-') {
                    *q = '_';
                }
            }
        } else {
            return 0;
        }
    }

    if (ptr->short_name && arg_parser_find_arg_desc(parser, ptr->short_name)) {
        return 0;
    }
    if (ptr->long_name && arg_parser_find_arg_desc(parser, ptr->long_name)) {
        return 0;
    }
    if (ptr->option_name && arg_parser_find_arg_desc(parser, ptr->option_name)) {
        return 0;
    }

    return 1;
}

static void free_arg_desc(struct arg_desc_t *desc) {
    free(desc->short_name);
    free(desc->long_name);
    free(desc->option_name);
    free(desc->desc);
    free(desc->metavar);
    free(desc->default_string);
    memset(desc, 0, sizeof(*desc));
    free(desc);
}

static struct arg_desc_t *clone_arg_desc(struct arg_desc_t *desc) {
    struct arg_desc_t *ptr;

    ptr = (struct arg_desc_t *) malloc(sizeof(*ptr));
    memcpy(ptr, desc, sizeof(*desc));
    ptr->long_name = CLONE_STRING(ptr->long_name);
    ptr->short_name = CLONE_STRING(ptr->short_name);
    ptr->option_name = CLONE_STRING(ptr->option_name);
    ptr->desc = CLONE_STRING(ptr->desc);
    ptr->metavar = CLONE_STRING(ptr->metavar);

    return ptr;
}

int arg_parser_add_argument(struct arg_parser_t *parser, struct arg_desc_t *desc) {
    struct arg_desc_t *ptr;

    DCHECK(desc);
    ptr = clone_arg_desc(desc);
    if (!check_arg_desc(parser, ptr)) {
        goto FAILURE;
    }

    list_add_tail(&ptr->node, &parser->arg_list);
    return 1;

FAILURE:
    free_arg_desc(ptr);
    return 0;
}

void close_arg_parser(struct arg_parser_t *parser) {
    struct arg_desc_t *ptr;

    LIST_FOR_EACH_ENTRY_SAFE(ptr, &parser->arg_list, node) {
        list_del(&ptr->node);
        free_arg_desc(ptr);
    }
    free(parser);
}

static int print_arg_desc(FILE *fp, struct arg_desc_t *desc) {
    int n = 0;
    if (!desc->is_required) {
        n += fprintf(fp, "[");
    }

    int type = desc->type;
    if (type != arg_type_positions) {
        if (desc->long_name) {
            n += fprintf(fp, "%s", desc->long_name);
        } else if (desc->short_name) {
            n += fprintf(fp, "%s", desc->short_name);
        }
    }

    if (type == arg_type_string || type == arg_type_list || type == arg_type_positions) {
        n += fprintf(fp, " %s", desc->metavar);
        if (type == arg_type_list || type == arg_type_positions) {
            n += fprintf(fp, " [%s]...", desc->metavar);
        }
    }

    if (!desc->is_required) {
        n += fprintf(fp, "]");
    }
    return n;
}

static void print_arg_parser_desc(FILE *fp, struct arg_parser_t *parser, const char *exec_name) {
    int n;
    struct arg_desc_t *ptr;

    n = fprintf(fp, "%s ", exec_name);
    LIST_FOR_EACH_ENTRY(ptr, &parser->arg_list, node) {
        if (n == 0) {
            n += fputs("        ", fp);
        }

        n += print_arg_desc(fp, ptr);
        n += fprintf(fp, " ");
        if (n >= 100) {
            n += fputs("\n", fp);
            n = 0;
        }
    }
    fputc('\n', fp);
}

/**
* 输出参数的详细介绍
*/
static void print_arg_desc_info(FILE *fp, struct arg_desc_t *ptr) {
    int size, type;
    int max, line;
    char *stack[2], *desc, *end;
    char buf[NBUF_SIZE];
    struct string_buffer_t buffer;

    type = ptr->type;
    max = 0;
    size = 0;
    if (ptr->long_name) {
        string_buffer_init(&buffer);
        string_buffer_printf(&buffer, "    %s", ptr->long_name);
        if (type != arg_type_int_switch) {
            string_buffer_printf(&buffer, " %s", ptr->metavar);
        }
        if (type == arg_type_list) {
            string_buffer_printf(&buffer, " [%s]", ptr->metavar);
        }
        stack[size++] = buffer.buf;
        max = MAX(max, buffer.size);
    }

    if (ptr->short_name) {
        string_buffer_init(&buffer);
        string_buffer_printf(&buffer, "    %s", ptr->short_name);
        if (type != arg_type_int_switch) {
            string_buffer_printf(&buffer, " %s", ptr->metavar);
        }
        if (type == arg_type_list) {
            string_buffer_printf(&buffer, " [%s]", ptr->metavar);
        }
        stack[size++] = buffer.buf;
        max = MAX(max, buffer.size);
    }
    max += 1;
    max = MAX(max, 40);
    snprintf(buf, NBUF_SIZE, "%%-%ds", max);

    desc = ptr->desc;
    line = 0;
    while (line < size || (desc && *desc != '\0')) {
        if (line < size) {
            fprintf(fp, buf, stack[line++]);
        } else {
            fprintf(fp, buf, "");
        }

        if (desc && *desc != '\0') {
            end = strchr(desc, '\n');
            if (!end) {
                end = desc + strlen(desc);
            }
            fwrite(desc, end - desc, sizeof(char), fp);
            desc = *end == '\n' ? end + 1 : end;
        }
        fputc('\n', fp);
    }

    for (int i = 0; i < size; ++i) {
        free(stack[i]);
    }
}

void arg_parser_print_help(FILE *fp, struct arg_parser_t *parser, const char *exec_name) {
    struct arg_desc_t *desc;

    print_arg_parser_desc(fp, parser, exec_name);
    fputc('\n', fp);

    LIST_FOR_EACH_ENTRY(desc, &parser->arg_list, node) {
        if (desc->type != arg_type_positions) {
            print_arg_desc_info(fp, desc);
        }
    }
}

static int can_stop_option_node(struct arg_parser_option_node_t *ptr, int is_end) {
    int type = ptr->desc->type;
    DCHECK(type == arg_type_string || type == arg_type_list || type == arg_type_positions);

    switch (ptr->desc->type) {
        case arg_type_positions: {
            return is_end;
        }
        case arg_type_string: {
            return !list_empty(&ptr->option.list);
        }
        case arg_type_list: {
            return !list_empty(&ptr->option.list);
        }
    }
    return 0;
}

static int on_arg_parser_option_start(struct arg_parser_t *parser,
    struct arg_parser_options_t *options, const char *name,
    struct arg_parser_option_node_t **out) {

    struct string_buffer_t buf;
    struct arg_parser_option_node_t *curr;
    struct arg_desc_t *desc;
    struct v_string_arg_t *arg;
    const char *q;
    int r;

    string_buffer_init(&buf);
    string_buffer_append(&buf, name);
    q = strchr(buf.buf, '=');
    if (q) {
        buf.size = q - buf.buf;
        buf.buf[buf.size] = '\0';
    }

    curr = find_arg_parser_option_node(options, buf.buf);
    if (!curr) {
        desc = arg_parser_find_arg_desc(parser, buf.buf);
        if (!desc) {
            goto FAILURE;
        }
        curr = alloc_arg_parser_option_node();
        curr->desc = desc;
        list_add_tail(&curr->node, &options->list);
    }

    switch(curr->desc->type) {
        case arg_type_int_switch: {
            if (q) {
                goto FAILURE;
            }
            curr = NULL;
            break;
        }
        case arg_type_string: {
            if (q) {
                if (*q == '\0') {
                    goto FAILURE;
                }
                LIST_FOR_EACH_ENTRY_SAFE(arg, &curr->option.list, node) {
                    list_del(&arg->node);
                    free(arg->str);
                    free(arg);
                }

                arg = (struct v_string_arg_t*) malloc(sizeof(*arg));
                INIT_LIST_HEAD(&arg->node);
                arg->str = s_strdup(q);
                list_add_tail(&arg->node, &curr->option.list);
                curr = NULL;
            }
            break;
        }
        case arg_type_list: {
            if (q) {
                if (*q == '\0') {
                    goto FAILURE;
                }

                arg = (struct v_string_arg_t*) malloc(sizeof(*arg));
                INIT_LIST_HEAD(&arg->node);
                arg->str = s_strdup(q);
                list_add_tail(&arg->node, &curr->option.list);
            }
            break;
        }
    }

    *out = curr;
    r = 1;
    goto OUT;

FAILURE:
    r = 0;

OUT:
    string_buffer_free(&buf);
    return r;
}

// 开始一个位置参数
static int on_arg_parser_position_option_start(struct arg_parser_t *parser,
        struct arg_parser_options_t *options, const char *name,
        struct arg_parser_option_node_t **out) {

    struct arg_parser_option_node_t *curr;
    struct arg_desc_t *desc;
    struct v_string_arg_t *arg;

    desc = arg_parser_find_position_desc(parser);
    if (!desc) {
        goto FAILURE;
    }

    curr = find_arg_parser_option_node(options, desc->option_name);
    if (curr) {
        goto FAILURE;
    }
    if (*name == '\0') {
        goto FAILURE;
    }

    // 添加一个结果到options结果里面
    curr = alloc_arg_parser_option_node();
    curr->desc = desc;
    list_add_tail(&curr->node, &options->list);

    // 添加一个选项结果
    arg = (struct v_string_arg_t*) malloc(sizeof(*arg));
    INIT_LIST_HEAD(&arg->node);
    arg->str = s_strdup(name);
    list_add_tail(&arg->node, &curr->option.list);
    *out = curr;

    return 1;

FAILURE:
    return 0;
}

static int on_arg_parser_option_append(struct arg_parser_t *parser,
        struct arg_parser_options_t *options, const char *name,
        struct arg_parser_option_node_t **out) {

    struct arg_parser_option_node_t *curr;
    struct v_string_arg_t *arg;
    int type;

    curr = *out;
    type = curr->desc->type;
    DCHECK(type == arg_type_string || type == arg_type_list || type == arg_type_positions);

    if (*name == '\0') {
        goto FAILURE;
    }

    arg = (struct v_string_arg_t*) malloc(sizeof(*arg));
    INIT_LIST_HEAD(&arg->node);
    arg->str = s_strdup(name);
    list_add_tail(&arg->node, &curr->option.list);

    if (type == arg_type_string) {
        *out = NULL;
    }

    return 1;

FAILURE:
    return 0;
}

static int arg_parser_check_required_options(struct arg_parser_t *parser,
        struct arg_parser_options_t *options) {
    return 1;
}

struct arg_parser_options_t *arg_parser_parse_args(struct arg_parser_t *parser,
    char **str, int start, int end) {

    struct arg_parser_options_t *options;
    struct arg_parser_option_node_t *curr;
    char *name;

    if (end <= start) {
        return NULL;
    }

    options = (struct arg_parser_options_t *) malloc(sizeof(*options));
    memset(options, 0, sizeof(*options));
    INIT_LIST_HEAD(&options->list);
    options->exec_name = s_strdup(str[start]);

    curr = NULL;
    for (int i = start + 1; i < end; ++i) {
        name = str[i];
        if (s_startwith(name, "--") || s_startwith(name, "-")) {
            if (curr && !can_stop_option_node(curr, 0)) {
                goto FAILURE;
            }

            curr = NULL;
            if (!on_arg_parser_option_start(parser, options, name, &curr)) {
                goto FAILURE;
            }
        } else if (!curr) {
           if (!on_arg_parser_position_option_start(parser, options, name, &curr)) {
                goto FAILURE;
           }
        } else {
            if (!on_arg_parser_option_append(parser, options, name, &curr)) {
                goto FAILURE;
            }
        }
    }

    if (curr && !can_stop_option_node(curr, 1)) {
        goto FAILURE;
    }

    if (!arg_parser_check_required_options(parser, options)) {
        goto FAILURE;
    }

out:
    return options;

FAILURE:
    if (options) {
        close_arg_parser_result(options);
        options = NULL;
    }
    goto out;
}

/**
 * 释放其中一个参数
 */
static void free_arg_option(struct arg_option_t *ptr) {
    struct v_string_arg_t *arg;

    LIST_FOR_EACH_ENTRY_SAFE(arg, &ptr->list, node) {
        list_del(&arg->node);
        free(arg->str);
        free(arg);
    }
}

void close_arg_parser_result(struct arg_parser_options_t *option) {
    struct arg_parser_option_node_t *ptr;

    free(option->exec_name);
    option->exec_name = NULL;
    LIST_FOR_EACH_ENTRY_SAFE(ptr, &option->list, node) {
       list_del(&ptr->node);
       free_arg_parser_option_node(ptr);
    }
    free(option);
}

struct arg_option_t *arg_parser_options_get(struct arg_parser_options_t *options,
                                            const char *name) {
    struct arg_parser_option_node_t *ptr;

    LIST_FOR_EACH_ENTRY(ptr, &options->list, node) {
        if (!strcmp(ptr->desc->option_name, name)) {
            return &ptr->option;
        }
    }
    return NULL;
}

/**
 * 申请一个选项结果
 */
struct arg_parser_option_node_t *alloc_arg_parser_option_node() {
    struct arg_parser_option_node_t *ptr;

    ptr = (struct arg_parser_option_node_t*) malloc(sizeof(*ptr));
    memset(ptr, 0, sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->node);
    INIT_LIST_HEAD(&ptr->option.node);
    INIT_LIST_HEAD(&ptr->option.list);
    return ptr;
}

void free_arg_parser_option_node(struct arg_parser_option_node_t *ptr) {
    ptr->desc = NULL;
    free_arg_option(&ptr->option);
    free(ptr);
}

struct arg_parser_option_node_t *find_arg_parser_option_node(
        struct arg_parser_options_t *options,
        const char *name) {
    struct arg_parser_option_node_t *ptr;

    LIST_FOR_EACH_ENTRY(ptr, &options->list, node) {
        if ((ptr->desc->long_name && !strcmp(name, ptr->desc->long_name))
            || (ptr->desc->short_name && !strcmp(name, ptr->desc->short_name))
            || (ptr->desc->option_name && !strcmp(name, ptr->desc->option_name))) {
            return ptr;
        }
    }
    return NULL;
}
