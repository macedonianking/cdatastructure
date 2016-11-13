#include "chapter4/tree_node.h"

#include <stdio.h>
#include <stdlib.h>

#include "macros.h"
#include "log.h"

struct tree_node_t *tree_single_rotation_left(struct tree_node_t *k1) {
	struct tree_node_t *k2;
	DCHECK(k1 && k1->left);
	k2 = k1->left;
	k1->left = k2->right;
	k2->right = k1;
	return k2;
}

struct tree_node_t *tree_double_rotation_left(struct tree_node_t *k1) {
	DCHECK(k1 && k1->left && k1->right);
	k1->left = tree_single_rotation_right(k1->left);
	return tree_single_rotation_left(k1);
}

struct tree_node_t *tree_single_rotation_right(struct tree_node_t *k1) {
	DCHECK(k1 && k1->right);
	struct tree_node_t *k2 = k1->right;
	k1->right = k2->left;
	k2->left = k1;
	return k2;
}

struct tree_node_t *tree_double_rotation_right(struct tree_node_t *k1) {
	DCHECK(k1 && k1->right && k1->right->left);
	k1->right = tree_single_rotation_left(k1->right);
	return tree_single_rotation_right(k1);
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
		ptr->data = tmp->data;
		ptr->right = del_tree_node(tmp->data, ptr->right);
	} else {
		tmp = ptr->left;
		if (!ptr->left) {
			tmp = ptr->right;
		}
		ptr->data = tmp->data;
		free(tmp);
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

void dump_tree_node_inorder(struct tree_node_t *ptr) {
	if (ptr) {
		fprintf(stdout, "%d ", ptr->data);
		dump_tree_node_inorder(ptr->left);
		dump_tree_node_inorder(ptr->right);
	}
}

int tree_node_count(struct tree_node_t *ptr) {
	int c = 0;
	if (ptr) {
		c = tree_node_count(ptr->left) + tree_node_count(ptr->right) + 1;
	}
	return c;
}

struct tree_node_t *find_tree_node_kth_min_impl(int k, int *kptr, struct tree_node_t *ptr) {
	struct tree_node_t *result = NULL, *temp;
	if (ptr) {
		temp = find_tree_node_kth_min_impl(k, kptr, ptr->left);
		if (temp != NULL) {
			result = temp;
		}
		if (*kptr < k) {
			++*kptr;
			result = ptr;
		}
		if (*kptr < k) {
			temp = find_tree_node_kth_min_impl(k, kptr, ptr);
			if (temp != NULL) {
				result = temp;
			}
		}
	}
	return result;
}

struct tree_node_t *find_tree_node_kth_min(int k, struct tree_node_t *ptr) {
	struct tree_node_t *result;
	int n = -1;
	result = find_tree_node_kth_min_impl(k, &n, ptr);
	return k == n ? result : NULL;
}