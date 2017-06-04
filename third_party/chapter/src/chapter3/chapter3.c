#include "chapter3.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils/utils.h"

#define STATE_DEFAULT   0
#define STATE_PREPARE   1
#define STATE_GETCHAR   2

#define CHAR_TYPE_DEFAULT   0
#define CHAR_TYPE_LOWER     1
#define CHAR_TYPE_UPPER     2
#define CHAR_TYPE_DIGIT     3
#define CHAR_TYPE_SEP       4

void chapter3_main(int argc, char **argv) {
    chapter3_exercise_3();
}

static inline int get_char_type(int ch) {
    if (ch == '-') {
        return CHAR_TYPE_SEP;
    } else if (isdigit(ch)) {
        return CHAR_TYPE_DIGIT;
    } else if (islower(ch)) {
        return CHAR_TYPE_LOWER;
    } else if (isupper(ch)) {
        return CHAR_TYPE_UPPER;
    } else {
        return CHAR_TYPE_DEFAULT;
    }
}

static inline int can_start_expand(int type) {
    return type == CHAR_TYPE_LOWER || type == CHAR_TYPE_UPPER || type == CHAR_TYPE_DIGIT;
}

static char *expand_impl(char *dst, char start, char end) {
    while (start <= end) {
        *dst++ = start;
        ++start;
    }
    return dst;
}

static void expand(const char *str, char *dst) {
    int state = STATE_DEFAULT;
    int start_char;
    int char_type;

    for (;*str != '\0'; ++str) {
        switch(state) {
            case STATE_DEFAULT: {
                char_type = get_char_type(*str);
                if (can_start_expand(char_type)) {
                    start_char = *str;
                    state = STATE_PREPARE;
                } else {
                    *dst++ = *str;
                }
                break;
            }
            case STATE_PREPARE: {
                char_type = get_char_type(*str);
                if (char_type == CHAR_TYPE_SEP) {
                    state = STATE_GETCHAR;
                } else {
                    *dst++ = start_char;
                    if (can_start_expand(char_type)) {
                        start_char = *str;
                    } else {
                        *dst++ = *str;
                        state = STATE_DEFAULT;
                    }
                }
                break;
            }
            case STATE_GETCHAR: {
                char_type = get_char_type(*str);
                if (char_type == get_char_type(start_char)) {
                    dst = expand_impl(dst, start_char, *str);
                    state = STATE_DEFAULT;
                } else {
                    *dst++ = start_char;
                    *dst++ = '-';
                    if (can_start_expand(char_type)) {
                        start_char = *str;
                        state = STATE_PREPARE;
                    } else {
                        *dst++ = *str;
                        state = STATE_DEFAULT;
                    }
                }
                break;
            }
        }
    }

    if (state == STATE_GETCHAR) {
        *dst++ = start_char;
        *dst++ = '-';
    } else if (state == STATE_PREPARE) {
        *dst++ = start_char;
    }
    *dst = '\0';
}

void chapter3_exercise_3() {
    char buf[NBUF_SIZE];

    buf[0] = '\0';

    expand("aaaaa-b-c", buf);
    printf("%s\n", buf);

    expand("a-z0-9", buf);
    printf("%s\n", buf);
}
