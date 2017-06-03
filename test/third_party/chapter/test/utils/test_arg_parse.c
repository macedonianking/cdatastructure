#include "test_arg_parse.h"

#include <memory.h>

#include "utils/test.h"
#include "utils/arg_parse.h"

static void test_case1();
static void test_add_argument();

void test_arg_parse() {
    test_case1();
}

void test_case1() {
    test_add_argument();
}

void test_add_argument() {
    struct arg_parser_t *parser;
    struct arg_desc_t desc;

    parser = open_arg_parser();

    memset(&desc, 0, sizeof(desc));
    desc.type = arg_type_list;
    desc.long_name = "--classpath";
    desc.short_name = "-C";
    desc.metavar = "PATH";
    desc.is_required = 1;

    TEST_CASE(ADD_ARG_PARSE_ARGUMENT, !arg_parser_add_argument(parser, &desc));
    TEST_CASE(ADD_ARG_ADD_SAME_NAME, arg_parser_add_argument(parser, &desc));

    desc.type = arg_type_list;
    desc.long_name = "--bootclasspath";
    desc.metavar = "PATH";
    desc.is_required = 0;
    TEST_CASE(ADD_ARG_PARSE_ARGUMENT, !arg_parser_add_argument(parser, &desc));

    TEST_VOID(ARG_PARSE_PRINT_HELP, arg_parser_print_help(stdout, parser, "javac"));

    close_arg_parser(parser);
}
