#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kdebug.h"
#include "string_ext.h"

#define MAX_COLUMNS		10
#define MAX_INPUT		1000
#define MAX_BUFFER_SIZE	1024

static int read_columns(int *columns, int count) {
	int n;
	int ch;

	n = 0;
	printf("Input columns settings, end with a negative integer:\n");
	while (n < count && scanf("%d", &columns[n]) && columns[n] > 0) {
		++n;
	}
	
	if (n == 0) {
		puts("columns must be set\n");
		exit(EXIT_FAILURE);
	}
	if ((n % 2) != 0) {
		puts("columns must be pair\n");
		exit(EXIT_FAILURE);
	}

	// discard all chars after settings.
	while ((ch = getchar()) != '\n' && ch != EOF) {
		;
	}
	return n;
}

static void rearrange_columns(char *output, const char *input, int *columns, int ncolumns) {
	const char *start, *ptr;

	start = input;
	*output = '\0';
	for (int j = 0; j < ncolumns && *start != '\0'; ++j) {
		for (ptr = start; *ptr != ',' && *ptr != '\0'; ++ptr)
			;
		if (ptr - start > columns[j]) {
			start = ptr - columns[j];
		}
		memcpy(output, start, ptr - start);
		output += ptr - start;
		*output = *ptr;
		if (*ptr == ',') {
			*++output = '\0';
			start = ++ptr;
		} else {
			start = ptr;
		}
	}
}

void chapter1_test_columns() {
	int columns[MAX_COLUMNS];
	char input[MAX_INPUT];
	char output[MAX_INPUT];
	int ncolumns;

	ncolumns = read_columns(columns, MAX_COLUMNS);
	while (gets(input) != NULL) {
		printf("Original is %s\n", input);
		rearrange_columns(output, input, columns, ncolumns);
		printf("Rearanged is %s\n", output);
	}
}

void chapter1_7_5_problem(const char *name) {
	FILE *fp;
	float price;
	int quantity;
	char department[MAX_BUFFER_SIZE];

	if ((fp = fopen(name, "r")) == NULL) {
		printf("Open file failure[%s]", name);
		exit(EXIT_FAILURE);
	}

	while (!feof(fp) && fscanf(fp, "%d %f", &quantity, &price)) {
		if (fgets(department, MAX_BUFFER_SIZE, fp)) {
			strtrim(department);
			printf("quantity=%d,\t\tprice=%f\t\t,department=%s\n", quantity, price, department);
		}
	}
	fclose(fp);
	fp = NULL;
}

static int read_lines_impl(struct line_space *ptr, FILE *fp) {
	char buffer[MAX_BUFFER_SIZE];

	while (fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL) {
		line_space_append(ptr, buffer);
		if (feof(fp)) {
			break;
		}
		if (ptr->length > 0 && ptr->data[ptr->length - 1] == '\n') {
			break;
		}
	}
	return !(feof(fp) && !ptr->length);
}

void chapter1_8_3_problem() {
	struct line_space space;
	int no;

	no = 0;
	init_line_space(&space);
	while (read_lines_impl(&space, stdin)) {
		printf("%2d:%s", no++, space.data);
		line_space_reset(&space);
	}
}

void chapter1_8_4_problem(FILE *fp) {
	char buffer[MAX_BUFFER_SIZE];
	int n;
	char checksum;

	checksum = -1;
	while (!feof(fp) && (n = fread(buffer, sizeof(char), MAX_BUFFER_SIZE, fp)) != 0) {
		for (int i = 0; i < n; ++i) {
			checksum += buffer[i];
		}
		fwrite(buffer, sizeof(char), n, stdout);
	}
	printf("%d\n", checksum);
}

void chapter1_8_5_problem(FILE *fp) {
	struct line_space max_line, current_line;

	init_line_space(&max_line);
	init_line_space(&current_line);
	while (read_lines_impl(&current_line, fp)) {
		if (current_line.length > max_line.length) {
			line_space_reset(&max_line);
			line_space_append(&max_line, current_line.data);
		}
		line_space_reset(&current_line);
	}
	printf("max line:\n%s", max_line.data);
}
