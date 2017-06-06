#include "list.h"

void list_move(struct list_head *ptr, struct list_head *head) {
	struct list_head *first, *last, *next;
	if (!list_empty(ptr)) {
		first = ptr->next;
		last = ptr->prev;
		INIT_LIST_HEAD(ptr);
		next = head->next;
		__list_head_link(head, first);
		__list_head_link(last, next);
	}
}

void list_move_tail(struct list_head *ptr, struct list_head *head) {
	struct list_head *first, *last, *prev;
	if (!list_empty(ptr)) {
		first = ptr->next;
		last = ptr->prev;
		INIT_LIST_HEAD(ptr);
		prev = head->prev;
		__list_head_link(prev, first);
		__list_head_link(last, head);
	}
}
