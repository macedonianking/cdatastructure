#include "chapter13.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "util.h"

static int isnonprint(int ch) {
	return !isprint(ch);
}

void chapter13_10_1_problem() {
	int sum;
	int (*method_array[])(int) = {
		iscntrl,
		isspace,
		isdigit,
		islower,
		isupper,
		ispunct,
		isnonprint,
	};
	char *descriptions[] = {
		"control",
		"space",
		"digit",
		"lower",
		"upper",
		"punct",
		"nonprint"
	};
	int counts[ARRAY_SIZE(method_array)];
	int ch;

	for (int i = 0; i < ARRAY_SIZE(counts); ++i) {
		counts[i] = 0;
	}

	sum = 0;
	while ((ch = getchar()) != EOF) {
		++sum;
		for (int i = 0; i < ARRAY_SIZE(counts); ++i) {
			if (method_array[i](ch)) {
				counts[i]++;
			}
		}
	}

	for (int i = 0; i < ARRAY_SIZE(counts); ++i) {
		printf("%s %.2f%%\n", descriptions[i], 100 * counts[i] / (float)sum);
	}

	sum = 0;
}
