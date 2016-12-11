#include "chapter3.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

struct nlist_node {
	struct list_head node;
	int data;
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

#if 0
static int plist_isordered(struct list_head *head) {
	struct plist_node *last, *curr;
	int r = 1;

	if (list_empty(head)) {
		return 1;
	}

	last = list_entry(head->next, typeof(*last), node);
	LIST_FOR_EACH_ENTRY_FROM(curr, last->node.next, head, node) {
		if (last->e <= curr->e) {
			r = 0;
			goto out;
		}
		last = curr;
	}
out:
	return r;
}
#endif

static void plist_add_item_decimal(struct list_head *head, int c, int e) {
	struct plist_node *ptr, *obj;
	struct list_head *node;

	LIST_FOR_EACH_ENTRY(ptr, head, node) {
		if (ptr->e > e) {
			continue;
		}

		if (ptr->e == e) {
			ptr->c += c;
			if (!ptr->c) {
				list_del(&ptr->node);
				free(ptr);
				return;
			}
			goto adjust;
		}
		node = &ptr->node;
		goto insert_item;
	}

	ptr = NULL;
	node = head;
insert_item:
	obj = (struct plist_node*) malloc(sizeof(struct plist_node));
	obj->c = c;
	obj->e = e;
	list_add_tail(&obj->node, node);

	ptr = obj;
adjust:
	while (abs(ptr->c) >= 10) {
		c = ptr->c / 10;
		e = ptr->e + 1;
		ptr->c = ptr->c % 10;
		node = ptr->node.prev;
		if (!ptr->c) {
			list_del(&ptr->node);
			free(ptr);
		}
		ptr = node != head ? list_entry(node, typeof(*ptr), node) : NULL;
		if (ptr == NULL) {
			ptr = (struct plist_node*) malloc(sizeof(struct plist_node));
			ptr->c = c;
			ptr->e = e;
			list_add(&ptr->node, node);
		} else if (ptr->e == e) {
			ptr->c += c;
			if (!ptr->c) {
				list_del(&ptr->node);
				free(ptr);
				return;
			}
		} else {
			// Must be (ptr->e >= e)
			obj = (struct plist_node*) malloc(sizeof(struct plist_node));
			obj->c = c;
			obj->e = e;
			list_add(&obj->node, &ptr->node);
			ptr = obj;
		}
	}
}

static void plist_multi_decimal(struct list_head *dst, struct list_head *lhs, struct list_head *rhs) {
	struct plist_node *lhsIter, *rhsIter;

	DCHECK(dst && list_empty(dst));
	LIST_FOR_EACH_ENTRY(lhsIter, lhs, node) {
		LIST_FOR_EACH_ENTRY(rhsIter, rhs, node) {
			plist_add_item_decimal(dst, lhsIter->c * rhsIter->c, lhsIter->e + rhsIter->e);
		}
	}
}

static void pow_plist_decimal(struct list_head *dst, struct list_head *list, int n) {
	DCHECK(n > 0);
	struct list_head lhs, rhs;

	if (n == 1) {
		duplicate_plist(dst, list);
	} else if ((n % 2) == 0) {
		INIT_LIST_HEAD(&lhs);
		pow_plist_decimal(&lhs, list, n / 2);
		plist_multi_decimal(dst, &lhs, &lhs);
		free_plist(&lhs);
	} else {
		INIT_LIST_HEAD(&lhs);
		INIT_LIST_HEAD(&rhs);

		pow_plist_decimal(&lhs, list, (n - 1) / 2);
		plist_multi_decimal(&rhs, &lhs, &lhs);
		free_plist(&lhs);
		plist_multi_decimal(dst, &rhs, list);
		free_plist(&rhs);
	}
}

static void print_plist_in_decimal(struct list_head *head) {
	struct plist_node *ptr;
	int e;

	if (list_empty(head)) {
		fputc('0', stdout);
		return;
	}

	ptr = list_entry(head->next, typeof(*ptr), node);
	e = ptr->e;
	fputc('0' + ptr->c, stdout);
	LIST_FOR_EACH_ENTRY_FROM(ptr, ptr->node.next, head, node) {
		while (--e > ptr->e) {
			fputc('0', stdout);
		}
		fputc('0' + ptr->c, stdout);
		e = ptr->e;
	}
	while (--e >= 0) {
		fputc('0', stdout);
	}
	fputc('\n', stdout);
}

void chapter3_9_problem() {
	struct list_head lhsHead, totalHead;

	INIT_LIST_HEAD(&lhsHead);
	INIT_LIST_HEAD(&totalHead);

	plist_add_item(&lhsHead, 2, 0);
	pow_plist_decimal(&totalHead, &lhsHead, 4000);
	dump_plist(&totalHead);
	print_plist_in_decimal(&totalHead);

	free_plist(&lhsHead);
	free_plist(&totalHead);
}

void chapter3_10_problem(int n, int m) {
	struct slist_node *head, **last, *ptr, *next;
	int k, isFirst;

	head = NULL;
	last = &head;

	DCHECK(n > 0 && m >= 0);
	for (int i = 1; i != 10; ++i) {
		ptr = (struct slist_node*) malloc(sizeof(struct slist_node));
		ptr->data = i;
		ptr->next = NULL;
		*last = ptr;
		last = &ptr->next;	
	}
	*last = head;

	ptr = head;
	k = m;
	isFirst = 1;
	while (ptr->next != ptr) {
		while (k-- > 0) {
			ptr = ptr->next;
		}
		next = ptr->next;
		if (isFirst) {
			printf("%d", ptr->data);
		} else {
			printf(" %d", ptr->data);
		}
		free(ptr);
		ptr = next;
		k = m;
	}
	if (isFirst) {
		printf("%d\n", ptr->data);
	} else {
		printf(" %d\n", ptr->data);
	}
}

struct slist_node **chapter3_11_problem_a(struct slist_node **head, int data) {
	while ((*head) && (*head)->data != data) {
		head = &(*head)->next;
	}
	return *head ? head : NULL;
}

struct slist_node **chapter3_11_problem_b(struct slist_node **head, int data) {
	if (*head == NULL) {
		return NULL;
	} else if((*head)->data == data) {
		return head;
	} else {
		return chapter3_11_problem_b(&(*head)->next, data);
	}
}

void chapter3_12_a_problem(struct slist_node **head) {
	struct slist_node *last, *next, *ptr;

	last = NULL;
	ptr = *head;
	while (ptr) {
		next = ptr->next;
		ptr->next = last;
		last = ptr;
		ptr = next;
	}
	*head = last;
}

#define BUCKET_SIZE	1000
#define PASS_MAX	3

static inline int digit_in(int n, int i) {
	if (n == 0) {
		return 0;
	} else if (i == 0) {
		return n % 10;
	} else {
		return digit_in(n / 10, i - 1);
	}
}

static void chapter3_13_problem_impl(struct list_head *head) {
	struct nlist_node *ptr;
	struct list_head bucket[BUCKET_SIZE];
	int n;

	for (int i = 0; i < BUCKET_SIZE; ++i) {
		INIT_LIST_HEAD(bucket+i);
	}

	for (int i = 0; i < PASS_MAX; ++i) {
		LIST_FOR_EACH_ENTRY_SAFE(ptr, head, node) {
			n = digit_in(ptr->data, i);
			list_del(&ptr->node);
			list_add_tail(&ptr->node, bucket + n);
		}
		DCHECK(list_empty(head));

		for (int j = 0; j < BUCKET_SIZE; ++j) {
			if (!list_empty(bucket+j)) {
				list_move_tail(bucket+j, head);
			}
		}
	}
}

static void free_nlist(struct list_head *head) {
	struct nlist_node *ptr;

	LIST_FOR_EACH_ENTRY_SAFE(ptr, head, node) {
		list_del(&ptr->node);
		free(ptr);
	}
}

static void dump_nlist(struct list_head *head) {
	struct nlist_node *ptr;
	int isFirst;

	isFirst = 1;
	LIST_FOR_EACH_ENTRY(ptr, head, node) {
		if (isFirst) {
			isFirst = 0;
			fprintf(stdout, "%d", ptr->data);
		} else {
			fprintf(stdout, " %d", ptr->data);
		}
	}
	fputc('\n', stdout);
}

void chapter3_13_problem() {
	int source[] = { 64, 8, 216, 512, 27, 729, 0, 1, 343, 125 };
	DEFINE_LIST_HEAD(head);
	struct nlist_node *ptr;

	for (int i = 0; i < NARRAY(source); ++i) {
		ptr = (struct nlist_node*) malloc(sizeof(struct nlist_node));
		ptr->data = source[i];
		list_add_tail(&ptr->node, &head);
	}
	chapter3_13_problem_impl(&head);
	dump_nlist(&head);
	free_nlist(&head);
}

#define SPLIT_TOKEN " \n\t"
struct employee_node {
	struct list_head node;
	char *name;
	int age;
};

static void split_token(char **ptr, int *n, int max, char *str) {
	int m;

	m = strspn(str, SPLIT_TOKEN);
	str = str + m;

	*n = 0;
	while (max > 0 && *str != '\0') {
		*ptr++ = str;
		++*n;
		--max;
		do {
			m = strcspn(str, SPLIT_TOKEN);
			str = str + m;
			if (max == 0) {
				*str = '\0';
				break;
			}
			m = strspn(str, SPLIT_TOKEN);
			if (m >= 2) {
				*str = '\0';
				str = str + m;
				break;
			}
			str = str + m;
		} while (*str != '\0');
	}
}

static struct employee_node *parse_employee_node(char *line) {
	char *tokens[2];
	int n, age;
	struct employee_node *ptr;

	split_token(tokens, &n, 2, line);
	if (n < 2 || sscanf(tokens[1], "%d", &age) != 1) {
		return NULL;
	}

	ptr = (struct employee_node*) malloc(sizeof(struct employee_node));
	ptr->name = strdup(tokens[0]);
	ptr->age = age;
	return ptr;
}

static void dump_employee_list(struct list_head *head) {
	struct employee_node *ptr;

	fprintf(stdout, "name\t\tage\n");
	LIST_FOR_EACH_ENTRY(ptr, head, node) {
		fprintf(stdout, "%s\t%d\n", ptr->name, ptr->age);
	}
}

static void free_employee_list(struct list_head *head) {
	struct employee_node *ptr;

	LIST_FOR_EACH_ENTRY_SAFE(ptr, head, node) {
		list_del(&ptr->node);
		free(ptr->name);
		ptr->name = NULL;
		free(ptr);
	}
}

void chapter3_14_a_problem(const char *filename) {
	FILE *fp;
	char buffer[MAX_BUFFER_SIZE];
	int n;
	struct list_head head;
	struct employee_node *ptr;

	INIT_LIST_HEAD(&head);
	if (!(fp = fopen(filename, "r"))) {
		snprintf(buffer, MAX_BUFFER_SIZE, "Error open file '%s'", filename);
		perror(buffer);
		exit(-1);
	}

	while (fgets(buffer, MAX_BUFFER_SIZE, fp) != NULL) {
		n = strlen(buffer);
		if (buffer[n - 1] != '\n' && !feof(fp)) {
			fprintf(stderr, "line is to long\n");
			goto finish;
		}
		ptr = parse_employee_node(buffer);
		if (ptr == NULL) {
			fprintf(stderr, "Error parsing %s\n", buffer);
			goto finish;
		}
		list_add_tail(&ptr->node, &head);
	}
	fclose(fp);
	fp = NULL;

	if (!list_empty(&head)) {
		dump_employee_list(&head);
		free_employee_list(&head);
	}
finish:
	if(fp) {
		fclose(fp);
	}
}

struct array_adjusting_list {
	int *data;
	int c;
	int n;
};

static void init_adjusting_list(struct array_adjusting_list* list) {
	list->c = 128;
	list->n = 0;
	list->data = (int*) malloc(list->c * sizeof(int));
	if (!list->data) {
		perror("Error allocating memory for init_adjusting_list");
		exit(-1);
	}
}

static void increment_adjusting_list_memory(struct array_adjusting_list *list, int c) {
	if (c <= list->c) {
		return;
	}

	int capacity;
	capacity = 1;
	while (capacity > 0 && capacity < c) {
		capacity <<= 1;
	}
	if (capacity < 0) {
		fputs("Allocating more memory for increment_adjusting_list_memory failure", stderr);
		exit(-1);
	}
	list->data = realloc(list->data, capacity * sizeof(int));
	if (!list->data) {
		perror("Allocating more memory for increment_adjusting_list_memory failure");
		exit(-1);
	}
	list->c = capacity;
}

static void add_array_adjusting_list(struct array_adjusting_list *list, int v) {
	int t;
	if (list->n == list->c) {
		increment_adjusting_list_memory(list, list->c + 1);
	}
	for (int i = 0; i <= list->n; ++i) {
		t = list->data[i];
		list->data[i] = v;
		v = t;
	}
	list->data[list->n++] = v;
}

static void find_array_adjusting_list(struct array_adjusting_list *list, int index) {
	int t;
	DCHECK(0 <= index && index < list->n);

	t = list->data[index];
	while (index-- > 0) {
		list->data[index+1] = list->data[index];
	}
	list->data[0] = t;
}

static void free_adjusting_list(struct array_adjusting_list *list) {
	free(list->data);
	list->data = NULL;
	list->n = 0;
	list->c = 0;
}

void chapter3_15_a_problem() {
	struct array_adjusting_list list;

	init_adjusting_list(&list);
	for (int i = 0; i < 100; ++i) {
		add_array_adjusting_list(&list, i);
	}
	find_array_adjusting_list(&list, 2);
	free_adjusting_list(&list);
}

void chapter3_16_b_problem(struct list_head *list) {
	struct nlist_node *ptr, *iter;
	struct list_head *iterNode, *nextNode;

	LIST_FOR_EACH_ENTRY(ptr, list, node) {
		iterNode = ptr->node.next;
		while (iterNode != list) {
			iter = list_entry(iterNode, struct nlist_node, node);
			nextNode = iter->node.next;
			if (ptr->data == iter->data) {
				list_del(&iter->node);
			}
			iterNode = nextNode;
		}
	}	
}

void chapter3_16_e_problem_iter(int *data, int start, int end, int *pend) {
	int m, endLeft, endRight, t;
	if (start == end) {
		*pend = start;
		return;
	}

	m = start + (end - start) / 2;
	chapter3_16_e_problem_iter(data, start, m, &endLeft);
	chapter3_16_e_problem_iter(data, m+1, end, &endRight);

	m = m + 1;
	for (int i = start; i <= endLeft; ++i) {
		for (int j = m; j <= endRight; j++) {
			if (data[i] == data[j]) {
				// elimiate
				t = data[endRight];
				data[endRight] = data[j];
				data[j] = t;
				--endRight;
				break;
			}
		}
	}
	endLeft += 1;
	if (endLeft == m) {
		endLeft = endRight;
		*pend = endLeft;
		return;
	}
	for(int i = m; i <= endRight; ++i) {
		data[endLeft++] = data[i];
	}
	*pend = endRight;
}

// 这里算法复杂度不是NlogN,不知道实现
void chapter3_16_e_problem(int *data, int n) {
	int end, isFirst;

	chapter3_16_e_problem_iter(data, 0, n - 1, &end);
	isFirst = 1;
	for (int i = 0; i <= end; ++i) {
		if (isFirst) {
			fprintf(stdout, "%d", data[i]);
		} else {
			fprintf(stdout, " %d", data[i]);
		}
	}
}

struct expression_node {
	struct list_head node;
	int type;
	int data;
};

void chapter3_19_problem() {

}
