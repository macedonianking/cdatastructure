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

struct tree_node_t *find_tree_node(int data, struct tree_node_t *ptr) {
	if (!ptr) {
		return NULL;
	}
	else if(data == ptr->data) {
		return ptr;
	} else if (data < ptr->data) {
		return find_tree_node(data, ptr->left);
	} else {
		return find_tree_node(data, ptr->right);
	}
}

struct tree_node_t *find_tree_node_min(struct tree_node_t *ptr) {
	if (!ptr) {
		return NULL;
	} else if(ptr->left) {
		return find_tree_node_min(ptr->left);
	} else {
		return ptr;
	}
}

struct tree_node_t *find_tree_node_max(struct tree_node_t *ptr) {
	if (!ptr) {
		return NULL;
	} else if (ptr->right) {
		return find_tree_node_max(ptr->right);
	} else {
		return ptr;
	}
}

struct tree_node_t *add_tree_node(int data, struct tree_node_t *ptr) {
	if (ptr == NULL) {
		ptr = (struct tree_node_t*) malloc(sizeof(struct tree_node_t));
		if (ptr == NULL) {
			fatal_error("Mallc memory for struct tree_node_t in add_tree_node %s %d", __FILE__, __LINE__);
		}
		ptr->data = data;
		ptr->left = ptr->right = NULL;
	} else {
		DCHECK(data != ptr->data);
		if (data < ptr->data) {
			ptr->left = add_tree_node(data, ptr->left);
		} else {
			ptr->right = add_tree_node(data, ptr->right);
		}
	}
	return ptr;
}

/**
 * It's the caller's responsibility to free node.
 */
struct tree_node_t *del_tree_node(int data, struct tree_node_t *ptr) {
	struct tree_node_t *tmp;
	if (!ptr) {
		return NULL;
	}
	if (data < ptr->data) {
		ptr->left = del_tree_node(data, ptr->left);
	} else if (data > ptr->data) {
		ptr->right = del_tree_node(data, ptr->right);
	} else if (ptr->left && ptr->right) {
		tmp = find_tree_node_min(ptr->right);
		ptr->right = del_tree_node(tmp->data, ptr->right);
		tmp->left = ptr->left;
		tmp->right = ptr->right;
		ptr = tmp;
	} else {
		tmp = ptr->left;
		if (!ptr->left) {
			tmp = ptr->right;
		}
		ptr = tmp;
	}
	return ptr;
}

void free_tree_nodes(struct tree_node_t *ptr) {
	if(ptr) {
		free_tree_nodes(ptr->left);
		free_tree_nodes(ptr->right);
		free(ptr);
	}
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
