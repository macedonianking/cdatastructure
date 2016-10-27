#include "chapter4/chapter4.h"

#include <stdio.h>
#include <stdlib.h>

#include "string_ext.h"
#include "macros.h"
#include "util.h"

void chapter4_14_7_problem() {
	char buffer[] = " dldldld aldldl     aldldlft\t ddlaldldf";
	string_deblank(buffer);
	printf("%s\n", buffer);
}

void chapter4_9_problem() {
	int data[] = {3, 1, 4, 6, 9, 2, 5, 7};
	struct tree_node_t *root, *ptr;

	root = NULL;
	for (int i = 0; i < ARRAY_SIZE(data); ++i) {
		root = add_tree_node(data[i], root);
	}

	ptr = find_tree_node(3, root);
	root = del_tree_node(3, root);
	free(ptr);
	free_tree_nodes(root);
}
