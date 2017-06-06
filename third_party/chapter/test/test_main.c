#include <stdio.h>

#include "test/test.h"
#include "test/test_utils.h"
#include "utils/test_arg_parse.h"

/**
* 主程序入口
*/
int main(int argc, char **argv) {
    ADD_TEST(TEST_UTILS, test_utils);
    ADD_TEST(TEST_ARG_PARSE, test_arg_parse);
    return 0;
}
