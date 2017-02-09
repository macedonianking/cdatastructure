#include "chapter14/chapter14_1.h"

/**
利用红黑树的基本数据结构,加上规模信息,取顺序统计量
*/
#include <stdlib.h>

#include "util.h"
#include "macros.h"

struct rb_node {
	struct rb_node *left, *right, *p;
	int key;
	int color;
	int size;
};

struct rb_tree {
	struct rb_node *root;
	struct rb_node nil;
};

enum {
	BLACK, RED
};

static void init_rb_tree(struct rb_tree *tree) {
	struct rb_node *nil = &tree->nil;

	tree->root = nil;
	nil->left = nil->right = nil->p = nil;
	nil->color = BLACK;
	nil->key = 0;
	nil->size = 0;
}

static void free_rb_tree_internal(struct rb_tree *tree, struct rb_node *node) {
	if (node != &tree->nil) {
		free_rb_tree_internal(tree, node->left);
		free_rb_tree_internal(tree, node->right);
		free(node);
	}
}

static void free_rb_tree(struct rb_tree *tree) {
	free_rb_tree_internal(tree, tree->root);
	tree->root = &tree->nil;
}

static struct rb_node *alloc_rb_node(int key) {
	struct rb_node *ptr = (struct rb_node*) malloc(sizeof(*ptr));
	DCHECK(ptr);
	ptr->key = key;
	ptr->color = BLACK;
	ptr->left = ptr->right = ptr->p = NULL;
	ptr->size = 0;
	return ptr;
}

static inline struct rb_node *rb_get_minimum(struct rb_tree *tree, struct rb_node *node) {
	if (node != &tree->nil) {
		while (node->left != &tree->nil) {
			node = node->left;
		}
	}
	return node != &tree->nil ? node : NULL;
}

static inline struct rb_node *rb_find(struct rb_tree *tree, int key) {
	struct rb_node *node;
	node = tree->root;
	while (node != &tree->nil && node->key != key) {
		if (key < node->key) {
			node = node->left;
		} else {
			node = node->right;
		}
	}
	return node != &tree->nil ? node : NULL;
}

static void rb_rotate_left(struct rb_tree *tree, struct rb_node *node) {
	struct rb_node *right;

	right = node->right;
	DCHECK(right != &tree->nil);
	node->right = right->left;
	if (node->right != &tree->nil) {
		node->right->p = node;
	}
	right->p = node->p;
	if (node->p == &tree->nil) {
		tree->root = right;
	} else if (node->p->left == node) {
		node->p->left = right;
	} else {
		node->p->right = right;
	}
	right->left = node;
	node->p = right;
	right->size = node->size;
	node->size = node->left->size + node->right->size + 1;
}

static void rb_rotate_right(struct rb_tree *tree, struct rb_node *node) {
	struct rb_node *left;

	left = node->left;
	DCHECK(left != &tree->nil);
	node->left = left->right;
	if (node->left != &tree->nil) {
		node->left->p = node;
	}
	left->p = node->p;
	if (node->p == &tree->nil) {
		tree->root = left;
	} else if (node->p->left == node) {
		node->p->left = left;
	} else {
		node->p->right = left;
	}
	left->right = node;
	node->p = left;
	left->size = node->size;
	node->size = node->left->size + node->right->size + 1;
}

static void rb_insert_fixup(struct rb_tree *tree, struct rb_node *node) {
	struct rb_node *w;

	while (node->p->color == RED) {
		if (node->p == node->p->p->left) {
			w = node->p->p->right;
			if (w->color == RED) {
				node->p->color = BLACK;
				w->color = BLACK;
				w->p->color = RED;
				node = w->p;
			} else if (node == node->p->right) {
				node = node->p;
				rb_rotate_left(tree, node);
			} else {
				node->p->color = BLACK;
				node->p->p->color = RED;
				rb_rotate_right(tree, node->p->p);
			}
		} else {
			w = node->p->p->left;
			if (w->color == RED) {
				w->color = BLACK;
				node->p->color = BLACK;
				node->p->p->color = RED;
				node = node->p->p;
			} else if (node == node->p->left) {
				node = node->p;
				rb_rotate_right(tree, node);
			} else {
				node->p->p->color = RED;
				node->p->color = BLACK;
				rb_rotate_left(tree, node->p->p);
			}
		}
	}
	tree->root->color = BLACK;
}

static void rb_tree_insert(struct rb_tree *tree, int key) {
	struct rb_node *p;
	struct rb_node *node;

	p = &tree->nil;
	node = tree->root;
	while (node != &tree->nil) {
		p = node;
		if (node->key == key) {
			return;
		} else if (key < node->key) {
			node = node->left;
		} else {
			node = node->right;
		}
	}

	node = alloc_rb_node(key);
	node->p = p;
	node->left = node->right = &tree->nil;
	node->color = RED;
	node->size = 1;
	if (p == &tree->nil) {
		tree->root = node;
	} else if (key < p->key) {
		p->left = node;
	} else {
		p->right = node;
	}
	while (p != &tree->nil) {
		++p->size;
		p = p->p;
	}
	rb_insert_fixup(tree, node);
}

static inline void rb_transplant(struct rb_tree *tree, struct rb_node *src, 
								 struct rb_node *dst) {
	if (src->p == &tree->nil) {
		tree->root = dst;
	} else if (src->p->left == src) {
		src->p->left = dst;
	} else {
		src->p->right = dst;
	}
	dst->p = src->p;
}

static void rb_delete_fixup(struct rb_tree *tree, struct rb_node *node) {
	struct rb_node *w;
	while (node != tree->root && node->color == BLACK) {
		if (node == node->p->left) {
			w = node->p->right;
			if (w->color == RED) {
				node->p->color = RED;
				w->color = BLACK;
				rb_rotate_left(tree, node->p);
			} else if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				node = node->p;
			} else if (w->left->color == RED && w->right->color == BLACK) {
				w->color = RED;
				w->left->color = BLACK;
				rb_rotate_right(tree, w);
			} else {
				w->color = node->p->color;
				node->p->color = BLACK;
				w->right->color = BLACK;
				rb_rotate_left(tree, node->p);
				node = tree->root;
			}
		} else {
			w = node->p->left;
			if (w->color == RED) {
				w->color = BLACK;
				node->p->color = RED;
				rb_rotate_right(tree, node->p);
			} else if (w->left->color == BLACK && w->right->color == BLACK) {
				w->color = RED;
				node = node->p;
			} else if (w->left->color == BLACK && w->right->color == RED) {
				w->color = RED;
				w->right->color = BLACK;
				rb_rotate_left(tree, w);
			} else {
				w->color = node->p->color;
				node->p->color = w->left->color = BLACK;
				rb_rotate_right(tree, node->p);
				node = tree->root;
			}
		}
	}
	node->color = BLACK;
}

static void rb_tree_delete_node(struct rb_tree *tree, struct rb_node *node) {
	struct rb_node *t;

	if (node->left != &tree->nil && node->right != &tree->nil) {
		t = rb_get_minimum(tree, node->right);
		node->key = t->key;
		rb_tree_delete_node(tree, t);
	} else {
		t = node->left;
		if (t == &tree->nil) {
			t = node->right;
		}
		rb_transplant(tree, node, t);
		rb_delete_fixup(tree, t);
		free(node);
	}
}

static void rb_tree_delete(struct rb_tree *tree, int key) {
	struct rb_node *node;

	node = rb_find(tree, key);
	if (node != NULL) {
		rb_tree_delete_node(tree, node);
	}
}

static struct rb_node *rb_tree_get_at_impl(struct rb_tree *tree, 
										   struct rb_node *node,
										   int i) {
	int r = node->left->size + 1;
	if (node == &tree->nil) {
		return NULL;
	}

	if (i == r) {
		return node;
	} else if (i < r) {
		return rb_tree_get_at_impl(tree, node->left, i);
	} else {
		return rb_tree_get_at_impl(tree, node->right, i-r);
	}
}

static inline struct rb_node *rb_tree_get_at(struct rb_tree *tree, int i) {
	return tree->root != &tree->nil ? rb_tree_get_at_impl(tree, tree->root, i) : NULL;
}

static inline int rb_tree_get_rank(struct rb_tree *tree, struct rb_node *node) {
	DCHECK(node != &tree->nil);

	int r = node->left->size + 1;
	while (node != tree->root) {
		if (node == node->p->right) {
			r += node->p->left->size + 1;
		}
		node = node->p;
	}
	return r;
}

void chapter13_insert() {
	struct rb_tree tree;
	int buf[] = {1, 3, 5, 7, 9};

	init_rb_tree(&tree);
	for (int i = 0; i < NARRAY(buf); ++i) {
		rb_tree_insert(&tree, buf[i]);
	}
	for (int i = 0; i < NARRAY(buf); ++i) {
		rb_tree_delete(&tree, buf[i]);
	}
	free_rb_tree(&tree);
}

static inline struct rb_node* 
rb_tree_get_at_plain(struct rb_tree *tree, int i) {
	struct rb_node *node;
	int r;

	node = tree->root;
	while (node != &tree->nil) {
		r = node->left->size + 1;
		if (r == i) {
			break;
		} else if (i < r) {
			node = node->left;
		} else {
			i -= r;
			node = node->right;
		}
	}
	return node != &tree->nil ? node : NULL;
}

static inline
int chapter14_1_4_problem(struct rb_tree *tree, int key) {
	struct rb_node *node;
	int r;

	r = 0;
	node = tree->root;
	while (node != &tree->nil) {
		if (node->key == key) {
			r += node->left->size + 1;
			break;
		} else if (key < node->key) {
			node = node->left;
		} else {
			r += node->left->size + 1;
			node = node->right;
		}
	}
	return node != &tree->nil ? r : 0;
}

void chapter14_1_main() {

}
