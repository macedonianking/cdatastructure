#include "test_arg_parse.h"

#include "utils/test.h"
#include "utils/arg_parse.h"

static void test_case1();
// static void test_add_argument();

void test_arg_parse() {
    test_case1();
}

void test_case1() {
    TEST_CASE("TEST", strlen("Hello world") > 0);
}
