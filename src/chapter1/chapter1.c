#include "chapter1/chapter1.h"

#include <stdio.h>

#include "util.h"
#include "macros.h"

void chapter1_30_tutorial() {
    if (-1 < (unsigned char)1) {
        printf("ansi value preserving\n");
    } else {
        printf("unsigned presevering standard\n");
    }
}

// signed int is promoted to unsigned int.
void chapter1_31_tutorial() {
    int data_buf[] = {1, 3, 3, 4, 5};
    int d = -1;
    if (d < ARRAY_SIZE(data_buf)/sizeof(int) - 2) {
        printf("signed int converted to unsigned int\n");
    }
}

struct string_line_buffer {
    char *str;
    int n;
    int c;
};

static int extend_string_line_buffer(struct string_line_buffer *buffer) {
    unsigned int value;
    int r = 0;
    char *str;
    
    value = 1;
    while (value < (unsigned int)buffer->c) {
        value <<= 1;
    }

    if ((signed int) value < 0) {
        return r;
    }

    str = (char*) realloc(buffer->str, value * sizeof(int));
    if (str) {
        buffer->str = str;
        buffer->c = value;
        r = 1;
    }
    return r;
}

static int read_line_buffer(FILE *fp, struct string_line_buffer *buffer) {
    int r;
    char *str;

    DCHECK(buffer->str[buffer->n] == '\0');
    r = 1;
    for (;;) {
        if (buffer->c - buffer->n < 256) {
            r = extend_string_line_buffer(buffer);
            if (!r) {
                break;
            }
        }
        str = fgets(buffer->str + buffer->n, buffer->c - buffer->n, fp);
        if (!str && !feof(fp)) {
            r = 0;
            break;
        }
        buffer->n += strlen(buffer->str + buffer->n);
        if (feof(fp) || buffer->str[buffer->n - 1] == '\n') {
            break;
        }
    }

    return r;
}

static void init_line_buffer(struct string_line_buffer *buffer) {
    buffer->n = 0;
    buffer->c = 256;
    buffer->str = (char*) malloc(sizeof(char) * buffer->c);
    DCHECK(buffer->str);
}

static void free_line_buffer(struct string_line_buffer *buffer) {
    free(buffer->str);
    buffer->n = 0;
    buffer->c = 0;
}

void chapter1_50_tutorial() {
    struct string_line_buffer buffer;

    init_line_buffer(&buffer);
    while (read_line_buffer(stdin, &buffer)) {
        fputs(buffer.str, stdout);
        buffer.n = 0;
    }
    free_line_buffer(&buffer);
}
