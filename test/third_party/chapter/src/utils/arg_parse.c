#include "arg_parse.h"

#include <stdlib.h>
#include <memory.h>

#include "utils/string_ext.h"
#include "utils/utils.h"

struct arg_parser {
    struct list_head arg_list;
};

struct arg_parser *open_arg_parser() {
    struct arg_parser *parser = (struct arg_parser*) malloc(sizeof(struct arg_parser));
    INIT_LIST_HEAD(&parser->arg_list);
    return parser;
}

static struct arg_desc_t *arg_parser_find_arg_desc(struct arg_parser *parser,
        const char *option_name) {
    struct arg_desc_t *ptr;

    if (s_is_empty(option_name)) {
        return 0;
    }

    LIST_FOR_EACH_ENTRY(ptr, &parser->arg_list, node) {
        if (!strcmp(ptr->option_name, option_name)) {
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

static int get_option_name(char *buf, int buf_size, struct arg_desc_t *desc) {
    int len;

    if (desc->option_name) {
        len = strlen(desc->option_name);
        if (len > 0 && len < buf_size) {
            strcpy(buf, desc->option_name);
            return 0;
        } else if (len > 0) {
            return 1;
        }
    }

    if (desc->long_name) {
        len = strlen(desc->long_name);
        if (len > 2 && len + 2 < buf_size) {
            strcpy(buf, desc->long_name + 2);
            normalize_option_name(buf);
            return 0;
        }
        if (len == 0) {
            return -1;
        }
    }

    if (desc->short_name) {
        len = strlen(desc->short_name);
        if (len > 1 && len + 1 < buf_size) {
            strcpy(buf, desc->short_name + 1);
            normalize_option_name(buf);
            return 0;
        }
        return -1;
    }

    return 1;
}

/**
 * 检查参数的完整性
 */
static int check_arg_desc(struct arg_desc_t *desc) {
    if (!desc || desc->type == arg_type_none) {
        return -1;
    }
    if (s_is_empty(desc->short_name) && s_is_empty(desc->long_name)) {
        if (s_is_empty(desc->option_name) || desc->type != arg_type_positions) {
            return -1;
        }
    }
    if (desc->short_name && !s_startwith(desc->short_name, "-")) {
        return -1;
    }
    if (desc->long_name && !s_startwith(desc->long_name, "--")) {
        return -1;
    }
    if (!s_is_empty(desc->option_name) 
        && (s_startwith(desc->option_name, "-") || s_startwith(desc->option_name, "--"))) {
        return -1;
    }
    if (desc->type != arg_type_int_switch && s_is_empty(desc->metavar)) {
        return -1;
    }
    return 0;
}

/**
* Find the positons argument descrition.
*/
static struct arg_desc_t *arg_parser_get_positions_desc(struct arg_parser *parser) {
    struct arg_desc_t *ptr;

    LIST_FOR_EACH_ENTRY(ptr, &parser->arg_list, node) {
        if (ptr->type == arg_type_positions) {
            return ptr;
        }
    }
    return NULL;
}

static void free_arg_desc(struct arg_desc_t *desc) {
    if (desc->short_name) {
        free(desc->short_name);
        desc->short_name = NULL;
    }
    if (desc->long_name) {
        free(desc->long_name);
        desc->long_name = NULL;
    }
    if (desc->option_name) {
        free(desc->option_name);
        desc->option_name = NULL;
    }
    if (desc->desc) {
        free(desc->desc);
        desc->desc = NULL;
    }
    if (desc->metavar) {
        free(desc->metavar);
        desc->metavar = NULL;
    }
    if (desc->default_string) {
        free(desc->default_string);
        desc->default_string = NULL;
    }
    free(desc);
}

int arg_parser_add_argument(struct arg_parser *parser, struct arg_desc_t *desc) {
    struct arg_desc_t *ptr;
    char option_name[NBUF_SIZE];

    if (check_arg_desc(desc) || get_option_name(option_name, NBUF_SIZE, desc)) {
        return -1;
    }
    if (arg_parser_find_arg_desc(parser, option_name)) {
        // 有同名的参数
        return -1;
    }
    if (desc->type == arg_type_positions && arg_parser_get_positions_desc(parser)) {
        // 不能有查过1个的位置参数
        return -1;
    }

    ptr = (struct arg_desc_t*) malloc(sizeof(*ptr));
    INIT_LIST_HEAD(&ptr->node);

    memset(ptr, 0, sizeof(*ptr));
    ptr->type = desc->type;
    ptr->long_name = desc->long_name ? s_strdup(desc->long_name) : NULL;
    ptr->short_name = desc->short_name ? s_strdup(desc->short_name) : NULL;
    ptr->option_name = s_strdup(option_name);
    ptr->desc = desc->desc ? s_strdup(desc->desc) : NULL;
    ptr->is_required = desc->is_required;

    switch(ptr->type) {
        case arg_type_int_switch: {
            ptr->default_int = desc->default_int;
            break;
        }
        case arg_type_string: {
            ptr->default_string = desc->default_string ? s_strdup(desc->default_string) : NULL;
            break;
        }
    }

    list_add_tail(&ptr->node, &parser->arg_list);
    return 0;
}

void close_arg_parser(struct arg_parser *parser) {
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

static void print_arg_parser_desc(FILE *fp, struct arg_parser *parser, const char *exec_name) {
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
}

void arg_parser_print_help(FILE *fp, struct arg_parser *parser, const char *exec_name) {
    print_arg_parser_desc(fp, parser, exec_name);
}
