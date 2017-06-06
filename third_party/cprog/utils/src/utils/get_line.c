#include "get_line.h"

#include <stdlib.h>
#include <string.h>

#include "utils/utils.h"

struct get_line_t {
    char *buf;
    int capacity;
    int size;
    int seek;
};

struct get_line_t *open_getline() {
    struct get_line_t *ptr = (struct get_line_t*) malloc(sizeof(*ptr));
    ptr->buf = (char*) malloc(sizeof(char) * NBUF_SIZE);
    ptr->capacity = NBUF_SIZE;
    ptr->size = ptr->seek = 0;
    return ptr;
}

int read_getline(struct get_line_t *get, FILE *fp, char **out, int *nout) {
    int is_ended = 0;
    int n, r, nout_len, out_capacity;
    char *start, *end, *out_p;

    nout_len = 0;       // 字符串的总长度
    out_p = *out;
    out_capacity = *nout;
    if (!out_p) {
        out_p = (char*) malloc(NBUF_SIZE * sizeof(char));
        out_capacity = NBUF_SIZE;
    }

    while (!is_ended) {
        if (!get->size) {
            n = fread(get->buf, sizeof(char), get->capacity-1, fp);
            if (n == 0) {
                if (feof(fp)) {
                    r = nout_len > 0;
                } else {
                    r = 0;
                }
                break;
            }

            get->buf[n] = '\0';
            get->seek = 0;
            get->size = n;
        }

        // 分析字符
        start = get->buf + get->seek;
        for (end = start; *end != '\0' && *end != '\n'; ++end)
            ;
        if (*end == '\n') {
            end += 1;
            is_ended = 1;
        }

        n = end - start;
        if (n + nout_len + 1 > out_capacity) {
            do {
                out_capacity <<= 1;
            } while (out_capacity > 0 && n + nout_len + 1 > out_capacity);
            if (out_capacity <= 0) {
                exit(-1);
            }
            out_p = (char*) realloc(out_p, out_capacity);
        }
        strncpy(out_p + nout_len, start, n);
        nout_len += n;
        get->seek += n;
        if (get->seek == get->size) {
            get->size = 0;
            get->seek = 0;
        }
        r = 1;
    }

    out_p[nout_len] = '\0';
    *out = out_p;
    *nout = out_capacity;
    return r;
}

void close_getline(struct get_line_t *ptr) {
    free(ptr->buf);
    ptr->buf = NULL;
    ptr->capacity = ptr->size = 0;
    free(ptr);
}
