#include "string_ext.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

#include "util.h"

#define IS_CHAR_EMPTY(ch) \
	({char _ch = (ch); _ch == '\n' ||_ch == ' ' || _ch == '\t';})

char *strtrim_impl(char *str) {
	char *base, *ptr;

	while (*str != '\0' && IS_CHAR_EMPTY(*str)) {
		++str;
	}

	base = NULL;
	for (ptr = str; *ptr != '\0'; ++ptr) {
		if (IS_CHAR_EMPTY(*ptr)) {
			if (base == NULL)
				base = ptr;
		} else {
			base = NULL;
		}
	}
	if (base != NULL) {
		*base = '\0';
	}
	return str;
}

void strtrim(char *str) {
	char *tmp = strtrim_impl(str);
	if (tmp != str) {
		strcpy(str, tmp);
	}
}

void init_line_space(struct line_space *ptr) {
	ptr->max_size = 1024;
	ptr->data = (char *) malloc(sizeof(char) * ptr->max_size);
	ptr->length = 0;
	ptr->data[0] = '\0';
	if (!ptr->data) {
		fprintf(stderr, "init_line_space failure.size=%d\n", ptr->max_size);
		exit(EXIT_FAILURE);
	}
}

void line_space_reset(struct line_space *ptr) {
	ptr->length = 0;
	ptr->data[0] = '\0';
}

void line_space_extend(struct line_space *ptr, int n) {
	if (ptr->max_size >= n) {
		return;
	}
	ptr->max_size = MAX(ptr->max_size * 2, n);
	ptr->data = realloc(ptr->data, ptr->max_size * sizeof(char));
	if (!ptr->data) {
		fprintf(stderr, "line_space_extend failure. size=%d\n", ptr->max_size);
		exit(EXIT_FAILURE);
	}
}

void line_space_append(struct line_space *ptr, const char *str) {
	int n = strlen(str);
	if (ptr->max_size < n + ptr->length + 1) {
		line_space_extend(ptr, n + ptr->length + 1);
	}
	strcpy(&ptr->data[ptr->length], str);
	ptr->length += n;
}

void string_deblank(char *str) {
	char *ptr;

	ptr = str;
	if (*ptr == '\0') {
		return;
	}

	while (*++str != '\0') {
		if (!IS_CHAR_EMPTY(*str)) {
			*++ptr = *str;
		} else if (!IS_CHAR_EMPTY(*ptr)) {
			*++ptr = ' ';
		}
	}
	*++ptr = '\0';
}

char *my_strrstr(const char *str, const char *src) {
	char *current, *last;

	if (!str || !src || *src == '\0') {
		return NULL;
	}

	last = NULL;
	current = strstr(str, src);
	while (current) {
		last = current;
		current = strstr(current+1, src);
	}
	return last;
}

char *strdup(const char *str) {
	int n = strlen(str);
	char *ptr;

	ptr = (char*) malloc(sizeof(char) * (n + 1));
	strcpy(ptr, str);
	return ptr;
}
