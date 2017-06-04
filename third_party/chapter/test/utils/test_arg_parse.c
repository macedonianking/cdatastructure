#include "test_arg_parse.h"

#include <memory.h>

#include "utils/test.h"
#include "utils/arg_parse.h"
#include "utils/utils.h"

static void test_case1();
static void test_add_argument();

static int compare_arg_string_list(struct list_head *list, char **buf, int size);

void test_arg_parse() {
    test_case1();
}

void test_case1() {
    test_add_argument();
}


void test_add_argument() {
    struct arg_parser_t *parser;
    struct arg_desc_t desc;
    struct arg_parser_options_t *options;
    struct arg_option_t *item;
    char *buf[NBUF_SIZE];
    int size, r;

    parser = open_arg_parser();

    memset(&desc, 0, sizeof(desc));
    desc.type = arg_type_list;
    desc.long_name = "--classpath";
    desc.short_name = "-C";
    desc.metavar = "PATH";
    desc.is_required = 1;
    desc.desc = "Speicifies the classpath.";

    TEST_CASE(ADD_ARG_PARSE_ARGUMENT, arg_parser_add_argument(parser, &desc));
    TEST_CASE(ADD_ARG_ADD_SAME_NAME, !arg_parser_add_argument(parser, &desc));

    desc.type = arg_type_string;
    desc.long_name = "--bootclasspath";
    desc.short_name = NULL;
    desc.metavar = "PATH";
    desc.is_required = 0;
    desc.desc = "Speicifies the bootclasspath.";
    TEST_CASE(ADD_ARG_PARSE_ARGUMENT, arg_parser_add_argument(parser, &desc));

    memset(&desc, 0, sizeof(desc));
    desc.type= arg_type_string;
    desc.long_name = "--source-version";
    desc.is_required = 1;
    desc.metavar = "VERSION_NAME";
    desc.desc = "Speicifies the source java version.";
    TEST_CASE(ADD_ARG_PARSE_ARGUMENT, arg_parser_add_argument(parser, &desc));

    arg_parser_print_help(stdout, parser, "javac");
    TEST_CASE(ARG_PARSE_PRINT_HELP, 1);

    size = 0;
    buf[size++] = "javac";
    buf[size++] = "--classpath";
    buf[size++] = "A";
    buf[size++] = "B";
    buf[size++] = "--source-version";
    buf[size++] = "1.7";

    options = arg_parser_parse_args(parser, buf, 0, size);
    TEST_CASE(ARG_PARSER_PARSE_ARGS, !!options);

    item = arg_parser_options_get(options, "classpath");
    r = 0;
    size = 0;
    buf[size++] = "A";
    buf[size++] = "B";
    if (item && compare_arg_string_list(&item->list, buf, 2)) {
        r = 1;
    }
    TEST_CASE(ARG_PARSER_PARSE_ARGS_CLASSPATH, r);

    item = arg_parser_options_get(options, "source_version");
    r = 0;
    size = 0;
    buf[size++] = "1.7";
    if(item && compare_arg_string_list(&item->list, buf, 1)) {
        r = 1;
    }
    TEST_CASE(ARG_PARSER_PARSE_ARGS_SOURCE_VERSION, r);

    close_arg_parser(parser);
}

int compare_arg_string_list(struct list_head *list, char **buf, int n) {
    struct v_string_arg_t *arg;
    int i = 0;

    LIST_FOR_EACH_ENTRY(arg, list, node) {
        if (i >= n) {
            return 0;
        }
        if (strcmp(arg->str, buf[i])) {
            return 0;
        }
        ++i;
    }

    return i == n;
}
