#include "chapter1/chapter1.h"

#include <stdio.h>

#include "utils/math_help.h"
#include "utils/utils.h"

#define LOWER   0
#define UPPER   300
#define STEP    20

void chapter_tutorial_1() {
    chapter_tutorial_1_6();
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
            ++ndigits[ch-'0'];
        } else if (ch == '\n' || ch == ' ' || ch == '\t') {
            ++nblanks;
        } else {
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
