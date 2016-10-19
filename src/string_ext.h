#ifndef STRING_EXT_H
#define STRING_EXT_H

struct line_space {
	char *data;
	int length;
	int max_size;
};

void init_line_space(struct line_space *ptr);
void line_space_reset(struct line_space *ptr);
void line_space_extend(struct line_space *ptr, int n);
void line_space_append(struct line_space *ptr, const char *str);

void strtrim(char *str);
char *strtrim_impl(char *str);
void string_deblank(char *str);
char *my_strrstr(const char *str, const char *src);

#endif // STRING_EXT_H
