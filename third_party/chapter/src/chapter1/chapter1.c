#include "chapter1/chapter1.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "utils/list.h"
#include "utils/math_help.h"
#include "utils/string_ext.h"
#include "utils/utils.h"

#define LOWER   0
#define UPPER   300
#define STEP    20

struct histogram_t {
    struct list_head node;
    int x;
    int y;
};

void chapter_tutorial_1(int argc, char **argv) {
    chapter_exercise_1_18(argc, argv);
}

/**
* 华氏度转摄氏度
*/
void chapter_exercise_1_3() {
    float fahr, cels;
    char buf[NBUF_SIZE];
    int fahr_count, cels_count;

    // 初始化
    cels = 5.0f / 9.0f * (UPPER - 32.0f);
    fahr_count = snprintf(buf, NBUF_SIZE, "%.0f", (float)UPPER);
    cels_count = snprintf(buf, NBUF_SIZE, "%.2f", cels);

    // 输出标题
    snprintf(buf, NBUF_SIZE, " %%%ds %%%ds\n", fahr_count, cels_count);
    printf(buf, "fahr", "cels");

    // 输出内容
    snprintf(buf, NBUF_SIZE, " %%%d.0f %%%d.2f\n", fahr_count, cels_count);
    for (fahr = 0.0f; fahr <= UPPER; fahr += STEP) {
        cels = 5.0f / 9.0f * (fahr - 32.0f);
        printf(buf, fahr, cels);
    }
}

/**
* 摄氏度转华氏度
*/
void chapter_exercise_1_5() {
    int cels;
    float fahr;
    char buf[NBUF_SIZE];
    int cels_count, fahr_count;

    // 初始化
    fahr = UPPER * 9.0f / 5.0f + 32.0f;
    fahr_count = snprintf(buf, NBUF_SIZE, "%.2f", fahr);
    cels_count = 4;

    // 输出标题
    snprintf(buf, NBUF_SIZE, " %%%ds %%%ds\n", cels_count, fahr_count);
    printf(buf, "cels", "fahr");

    // 输出内容
    snprintf(buf, NBUF_SIZE, " %%%dd %%%d.2f\n", cels_count, fahr_count);
    for (cels = UPPER; cels >= LOWER; cels -= STEP) {
        fahr = cels * 9.0f / 5.0f + 32.0f;
        printf(buf, cels, fahr);
    }
}

/**
* 获取字符
*/
void chapter_tutorial_1_5() {
    int ch;
    int line;

    line = 0;
    for (;;) {
        ch = getchar();
        if (ch == EOF) {
            break;
        }

        printf("%3d:", line);
        if (ch == '\n') {
            goto NEWLINE_LABLE;
        }
        while ((ch = getchar()) != EOF && ch != '\n') {
            putchar(ch);
        }

        if (ch == EOF) {
            break;
        }
    NEWLINE_LABLE:
        putchar(ch);
        ++line;
    }

}

void chapter_exercise_1_9() {
    int ch;
    int is_blank;

    is_blank = 0;
    while ((ch = getchar()) != EOF) {
        if (is_blank != (ch == ' ' || ch == 't')) {
            is_blank = !is_blank;
            putchar(ch);
        }
        else if (!is_blank) {
            putchar(ch);
        }
    }
}

void chapter_tutorial_1_6() {
    int ch;
    int nblanks, nothers;
    int ndigits[10];

    nblanks = nothers = 0;
    for (int i = 0; i < 10; ++i) {
        ndigits[i] = 0;
    }

    while ((ch = getchar()) != EOF) {
        if ('0' <= ch && ch <= '9') {
            ++ndigits[ch - '0'];
        }
        else if (ch == '\n' || ch == ' ' || ch == '\t') {
            ++nblanks;
        }
        else {
            ++nothers;
        }
    }

    printf("digits: ");
    for (int i = 0; i < 10; ++i) {
        printf("%d ", ndigits[i]);
    }
    printf("\n");
    printf("nblanks: %d\n", nblanks);
    printf("nothers: %d\n", nothers);
}

/**
*@param ch 输出的字符
*@param n 输出的总字符个数
*@param max 每行输出的最大字符个数
*@param prefix 前缀空格大小
*/
static inline void PrintChars(int ch, int n, int max, int prefix) {
    int output_prefix = 0;
    int count;

    while (n > 0) {
        if (output_prefix) {
            for (int i = 0; i < prefix; ++i)
                putchar(' ');
        }

        count = MIN(n, max);
        n -= count;
        for (int i = 0; i < count; ++i) {
            putchar(ch);
        }
        if (n > 0) {
            putchar('\n');
        }

        output_prefix = 1;
    }
}

static void PrintHorizontal(int nblanks, int nothers, int *ndigits) {
    const char *format = "%-6s : ";
    int prefix, nwhitespace;

    // 输出nblanks的个数
    prefix = printf(format, "nblansk");
    PrintChars('*', nblanks, 100, prefix);
    putchar('\n');

    // 输出nothers的个数
    prefix = printf(format, "nothers");
    PrintChars('*', nothers, 100, prefix);
    putchar('\n');

    // 输出个数
    format = "%-6s %d: ";
    prefix = printf(format, "ndigits", 0);
    nwhitespace = prefix - 3;
    PrintChars('*', ndigits[0], 100, prefix);
    putchar('\n');
    for (int i = 1; i < 10; ++i) {
        PrintChars(' ', nwhitespace, 100, 0);
        printf("%d: ", i);
        PrintChars('*', ndigits[i], 100, prefix);
        putchar('\n');
    }
}

#if 0

/*
* 格式化
*/
static void FormatHistogram(struct list_head *list, int *x, int max, int n) {
    int count;
    struct histogram_t *hist;

    while (n > 0) {
        count = MIN(n, max);
        n -= count;

        hist = (struct histogram_t*) malloc(sizeof(*hist));
        INIT_LIST_HEAD(&hist->node);
        hist->x = *x;
        *x += 1;
        hist->y = count;
        list_add_tail(&hist->node, list);
    }
}

/**
* 输出统计条
*/
static void PrintHisogram(struct list_head *list) {
    int max_x, max_y, x, y;
    struct histogram_t *ptr;

    max_x = 0;
    max_y = 0;
    LIST_FOR_EACH_ENTRY(ptr, list, node) {
        max_y = MAX(max_y, ptr->y);
        max_x = MAX(max_x, ptr->x);
    }

    for (y = 0; y < max_y; ++y) {
        x = -1;
        LIST_FOR_EACH_ENTRY(ptr, list, node) {
            while (x + 1 < ptr->x) {
                putchar(' ');
                ++x;
            }
            if (y < ptr->y) {
                putchar('*');
            }
            else {
                putchar(' ');
            }
            ++x;
        }
        putchar('\n');
    }
}

#endif 

void chapter_exercise_1_13() {
    int nblanks, nothers;
    int ndigits[10];
    int ch;

    nblanks = nothers = 0;
    for (int i = 0; i < 10; ++i) {
        ndigits[i] = 0;
    }

    while ((ch = getchar()) != EOF) {
        if ('0' <= ch && ch <= '9') {
            ++ndigits[ch - '0'];
        }
        else if (ch == '\n' || ch == '\t' || ch == ' ') {
            ++nblanks;
        }
        else {
            ++nothers;
        }
    }
    PrintHorizontal(nblanks, nothers, ndigits);
}

#define NCHAR_COUNT ('z' - 'a')

void chapter_exercise_1_14() {
    int nlower[NCHAR_COUNT];
    int nupper[NCHAR_COUNT];
    int ch;

    while ((ch = getchar()) != EOF) {
        if (ch >= 'a' && ch <= 'z') {
            ++nlower[ch - 'a'];
        } else if (ch >= 'A' && ch <= 'Z') {
            ++nupper[ch - 'A'];
        }
    }
    printf("nlower : ");
    for (int i = 0; i < NARRAY(nlower); ++i) {
        printf("%d ", nlower[i]);
    }
    printf("\n");    

    printf("nupper : ");
    for (int i = 0; i < NARRAY(nupper); ++i) {
        printf("%d ", nupper[i]);
    }
    printf("\n");
}

void chapter_tutorial_1_9(int argc, char **argv) {
    char *line, *max_line;
    size_t nline, max_line_capacity, max_line_length;
    int line_length;
    FILE *fp;

    if (argc < 2) {
        return;
    }

    if (!(fp = fopen(argv[1], "r"))) {
        return;
    }

    line = NULL;
    nline = 0;
    max_line_capacity = NBUF_SIZE;
    max_line_length = -1;
    max_line = (char*) malloc(sizeof(char) * max_line_capacity);

    while (getline(&line, &nline, fp) > 0) {
        line_length = strlen(line);
        if (line_length > max_line_length) {
            if (max_line_capacity < line_length + 1) {
                max_line_capacity = line_length + 1;
                max_line = (char*) realloc(max_line, max_line_capacity);
            }
            strcpy(max_line, line);
            max_line_length = line_length;
        }
    }

    fclose(fp);
    free(line);

    if (max_line_length >= 0) {
        printf("%s", max_line);
        if (max_line_length > 0 && max_line[max_line_length-1] != '\n') {
            putchar('\n');
        }
    }
    free(max_line);
    max_line = NULL;
}

void chapter_exercise_1_17(int argc, char **argv) {
    FILE *fp;
    char *line;
    size_t nline, line_length;
    DEFINE_LIST_HEAD(out_list);
    struct string_node_t *item;

    if (argc < 2) {
        return;
    }

    fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "can't open file '%s' error is '%s'\n", argv[1], strerror(errno));
        return;
    }

    line = NULL;
    nline = 0;

    while (getline(&line, &nline, fp) > 0) {
        line_length = strlen(line);
        if (line_length >= 80) {
            item = (struct string_node_t*) malloc(sizeof(*item));
            INIT_LIST_HEAD(&item->node);
            item->str = s_strdup(line);
            list_add_tail(&item->node, &out_list);
        }
    }

    fclose(fp);
    free(line);

    LIST_FOR_EACH_ENTRY(item, &out_list, node) {
        printf("%s", item->str);
    }

    LIST_FOR_EACH_ENTRY_SAFE(item, &out_list, node) {
        list_del(&item->node);
        free(item->str);
        free(item);
    }
}

/*
* 移除行尾的空格符
*/
static int remove_trailing_whitespace(char *str) {
    char *anchor, *ptr;

    anchor = NULL;
    for (ptr = str; *ptr != '\0'; ++ptr) {
        if (*ptr == '\n' || *ptr == '\t' || *ptr == ' ') {
            if (!anchor) {
                anchor = ptr;
            }
        } else {
            anchor =  NULL;
        }
    }
    if (anchor) {
        *anchor = '\0';
        ptr = anchor;
    }
    return ptr != str;
}

void chapter_exercise_1_18(int argc, char **argv) {
    FILE *fp;
    char *line = NULL;
    size_t nline = 0;

    if (argc < 2) {
        return;
    }

    if (!(fp = fopen(argv[1], "r"))) {
        fprintf(stderr, "can't open file '%s', error is '%s'\n", 
            argv[1], strerror(errno));
        return;
    }

    while (getline(&line, &nline, fp) > 0) {
        if (remove_trailing_whitespace(line)) {
            printf("%s\n", line);
        }
    }

    fclose(fp);
    fp = NULL;
    free(line);
}
