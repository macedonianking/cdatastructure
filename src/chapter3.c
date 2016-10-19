#include "chapter3.h"


#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "macros.h"

struct slist_node {
	struct slist_node *next;
	int data;
};

struct dlist_node {
	struct dlist_node *next;
	struct dlist_node *prev;
	int v;
};

struct plist_node {
	struct list_head node;
	int c;
	int e;
};


void chapter3_3_a(struct slist_node **head) {
	struct slist_node *curr, *next;

	if (!head || !(curr = *head) || !(next = curr->next)) {
		return;
	}

	*head = next;
	curr->next = next->next;
	next->next = curr;
}

void chapter3_3_b(struct dlist_node **head, struct dlist_node **tail, struct dlist_node *curr) {
	struct dlist_node *next, **pnext, **pprev;

	if (!curr || !(next = curr->next)) {
		return;
	}

	pnext = curr->prev ? &curr->prev->next : head;
	*pnext = next;
	next->prev = pnext == head ? NULL : curr->prev;

	pprev = next->next ? &next->next->prev : tail;
	*pprev = curr;
	curr->next = pprev == tail ? NULL : next->next;

	next->next = curr;
	curr->prev = next;
}

void chapter3_4_problem(struct slist_node **dst, struct slist_node *lhs, struct slist_node *rhs) {
	struct slist_node *head, **last, *ptr;
	int n;

	head = NULL;
	last = &head;

	while (lhs && rhs) {
		if (lhs->data == rhs->data) {
			n = lhs->data;
			ptr = (struct slist_node*) malloc(sizeof(struct slist_node));
			ptr->data = n;
			ptr->next = NULL;
			*last = ptr;
			last = &ptr->next;

			do {
				lhs = lhs->next;
			} while (lhs && lhs->data == n);
			do {
				rhs = rhs->next;
			} while (rhs && rhs->data == n);
			continue;
		} 
		else if (lhs->data < rhs->data) {
			do {
				lhs = lhs->next;
			} while (lhs && lhs->data < rhs->data);
		} else {
			do {
				rhs = rhs->next;
			} while (rhs && rhs->data < lhs->data);
		}
	}

	*dst = head;
}

void chapter3_5_problem(struct slist_node **dst, struct slist_node *lhs, struct slist_node *rhs) {
	struct slist_node *head, **last, *ptr;
	int n;

	head = NULL;
	last = &head;

	while (lhs || rhs) {
		ptr = NULL;
		if (!rhs || (lhs && lhs->data <= rhs->data)) {
			n = lhs->data;
		} else {
			n = rhs->data;
		}

		for (; lhs && lhs->data == n; lhs = lhs->next)
			;
		for (; rhs && rhs->data == n; rhs = rhs->next)
			;

		ptr = (struct slist_node*) malloc(sizeof(struct slist_node));
		ptr->next = NULL;
		ptr->data = n; 
		*last = ptr;
		last = &ptr->next;
	}
	*dst = head;
}


static void duplicate_plist(struct list_head *dst, struct list_head *head) {
	DCHECK(dst && list_empty(dst));
	struct plist_node *ptr, *headIter;

	LIST_FOR_EACH_ENTRY(headIter, head, node) {
		ptr = (struct plist_node*) malloc(sizeof(struct plist_node));
		ptr->c = headIter->c;
		ptr->e = headIter->e;
		list_add_tail(&ptr->node, dst);
	}
}

static void plist_add_item(struct list_head *head, int c, int e) {
	struct plist_node *ptr, *tmp;

	LIST_FOR_EACH_ENTRY(ptr, head, node) {
		if (ptr->e > e) {
			continue;
		}
		if (ptr->e == e) {
			ptr->c += c;
			if (!ptr->c) {
				list_del(&ptr->node);
				free(ptr);
			}
		} else {
			tmp = (struct plist_node*) malloc(sizeof(struct plist_node));
			tmp->c = c;
			tmp->e = e;
			list_add_tail(&tmp->node, &ptr->node);
		}
		return;
	}
	tmp = (struct plist_node*) malloc(sizeof(struct plist_node));
	tmp->c = c;
	tmp->e = e;
	list_add_tail(&tmp->node, head);
}

static void plist_add(struct list_head *dst, struct list_head *lhs, struct list_head *rhs) {
	struct plist_node *ptr;

	duplicate_plist(dst, lhs);
	LIST_FOR_EACH_ENTRY(ptr, rhs, node) {
		plist_add_item(dst, ptr->c, ptr->e);
	}
}

static void plist_multi(struct list_head *dst, struct list_head *lhs, struct list_head *rhs) {
	struct plist_node *lhsIter, *rhsIter;

	DCHECK(dst && list_empty(dst));
	LIST_FOR_EACH_ENTRY(lhsIter, lhs, node) {
		LIST_FOR_EACH_ENTRY(rhsIter, rhs, node) {
			plist_add_item(dst, lhsIter->c * rhsIter->c, lhsIter->e + rhsIter->e);
		}
	}
}

static void free_plist(struct list_head *head) {
	struct plist_node *ptr;

	LIST_FOR_EACH_ENTRY_SAFE(ptr, head, node) {
		list_del(&ptr->node);
		free(ptr);
	}
}

static void dump_plist(struct list_head *head) {
	int isFirst = 1;
	struct plist_node *ptr;

	LIST_FOR_EACH_ENTRY(ptr, head, node) {
		if (isFirst) {
			isFirst = 0;
			fprintf(stdout, "%dX^%d", ptr->c, ptr->e);
		} else {
			fprintf(stdout, " %c %dX^%d", ptr->c >= 0 ? '+' : '-', abs(ptr->c), ptr->e);
		}
	}
	fputc('\n', stdout);
}

static void pow_plist(struct list_head *dst, struct list_head *list, int n) {
	DCHECK(n > 0);
	struct list_head lhs, rhs;

	if (n == 1) {
		duplicate_plist(dst, list);
	} else if ((n % 2) == 0) {
		INIT_LIST_HEAD(&lhs);
		pow_plist(&lhs, list, n / 2);
		plist_multi(dst, &lhs, &lhs);
		free_plist(&lhs);
	} else {
		INIT_LIST_HEAD(&lhs);
		INIT_LIST_HEAD(&rhs);

		pow_plist(&lhs, list, (n - 1) / 2);
		plist_multi(&rhs, &lhs, &lhs);
		free_plist(&lhs);
		plist_multi(dst, &rhs, list);
		free_plist(&rhs);	
	}
}

void chapter3_6_problem() {
	struct list_head lhsHead, rhsHead, totalHead;

	INIT_LIST_HEAD(&lhsHead);
	INIT_LIST_HEAD(&rhsHead);
	INIT_LIST_HEAD(&totalHead);

	plist_add_item(&lhsHead, 1, 1);
	plist_add_item(&lhsHead, 1, 0);
	plist_add_item(&rhsHead, 1, 1);
	plist_add_item(&rhsHead, 1, 0);

	plist_add(&totalHead, &lhsHead, &rhsHead);
	dump_plist(&totalHead);
	free_plist(&lhsHead);
	free_plist(&rhsHead);
	free_plist(&totalHead);
}

void chapter3_7_problem() {
	struct list_head lhsHead, rhsHead, totalHead;

	INIT_LIST_HEAD(&lhsHead);
	INIT_LIST_HEAD(&rhsHead);
	INIT_LIST_HEAD(&totalHead);

	plist_add_item(&lhsHead, 1, 1);
	plist_add_item(&lhsHead, 1, 0);
	plist_add_item(&rhsHead, 1, 1);
	plist_add_item(&rhsHead, 1, 0);

	plist_multi(&totalHead, &lhsHead, &rhsHead);
	dump_plist(&totalHead);
	free_plist(&totalHead);
	free_plist(&lhsHead);
	free_plist(&rhsHead);
}

void chapter3_8_problem() {
	struct list_head head, totalHead;

	INIT_LIST_HEAD(&head);
	INIT_LIST_HEAD(&totalHead);

	plist_add_item(&head, 1, 1);
	plist_add_item(&head, 1, 0);
	pow_plist(&totalHead, &head, 4);
	dump_plist(&totalHead);
	free_plist(&totalHead);
}
