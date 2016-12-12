#include "chapter12.h"

#include <stdlib.h>

#include "util.h"

struct s_node_t {
	struct s_node_t *next;
	int n;
};

struct d_node_t {
	struct d_node_t *next;
	struct d_node_t *prev;
	int n;
};

void insert_single_link_list(struct s_node_t **head, int v) {
	struct s_node_t *ptr;

	while ((*head) && (*head)->n < v) {
		head = &(*head)->next;
	}
	ptr = (struct s_node_t*) malloc(sizeof(struct s_node_t));
	if (!ptr) {
		exit(1);
	}
	ptr->n = v;
	ptr->next = *head;
	*head = ptr;
}

void inverse_single_link_list(struct s_node_t **head, int n) {
	struct s_node_t **last, *ptr, *next;
	int stack_size;

	stack_size = 0;
	ptr = *head;
	while (ptr) {
		next = ptr->next;
		ptr->next = *head;
		*head = ptr;
		++stack_size;
		if (stack_size == 1) {
			last = &ptr->next;
		}
		if (stack_size == n) {
			head = last;
			stack_size = 0;
		}
		ptr = next;
	}
}

int count_sigle_list_nodes(struct s_node_t *head) {
	int count = 0;
	while (head) {
		++count;
	}
	return count;
}

struct s_node_t *search_in_unordered_sigle_list(struct s_node_t *head, int n) {
	while (head && head->n != n) {
		head = head->next;
	}
	return head;
}

struct s_node_t *search_in_ordred_single_list(struct s_node_t *head, int n) {
	while (head && head->n < n) {
		head = head->next;
	}
	return head != NULL && head->n == n ? head : NULL;
}

struct s_node_t *reverse_single_link_list(struct s_node_t *head) {
	struct s_node_t *ptr, *next;

	ptr = NULL;
	while (head) {
		next = head->next;
		head->next = ptr;
		ptr = head;
		head = next;
	}

	return ptr;
}

int single_link_list_remove_node(struct s_node_t **head, struct s_node_t *node) {
	struct s_node_t *ptr;
	int r = 0;

	while ((ptr = *head) && ptr != node) {
		head = &ptr->next;
	}
	if (ptr) {
		r = 1;
		*head = ptr->next;
		free(ptr);
	}
	return r;
}

void double_list_insert_for_asscending_order(struct d_node_t **head, struct d_node_t **tail, int n) {
	struct d_node_t **pptr, *ptr, *tmp;

	for (pptr = head; (ptr = *pptr) && ptr->n < n; pptr = &ptr->next)
		;
	if (ptr && ptr->n == n) {
		return;
	}
	tmp = (struct d_node_t*) malloc(sizeof(struct d_node_t));
	tmp->next = tmp->prev = NULL;
	tmp->n = n;
	if (ptr) {
		tmp->next = ptr;
		ptr->prev = tmp;
	} else {
		tmp->next = NULL;
		*tail = tmp;
	}
	if (head == pptr) {
		*head = tmp;
		tmp->prev = NULL;
	} else {
		ptr = CONTAINER_OF(pptr, struct d_node_t, next);
		ptr->next = tmp;
		tmp->prev = ptr;
	}
}
