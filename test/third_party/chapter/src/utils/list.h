#ifndef MAIN_LIST_H
#define MAIN_LIST_H

#include "utils.h"

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define INIT_LIST_HEAD(ptr) (ptr)->next = (ptr)->prev = (ptr)
#define DEFINE_LIST_HEAD(name) struct list_head name = {.next = &name, .prev=&name}

static inline void __list_head_link(struct list_head *prev, struct list_head *next) {
	prev->next = next;
	next->prev = prev;
}

static inline void __list_head_add(struct list_head *prev, struct list_head *next, struct list_head *ptr) {
	__list_head_link(prev, ptr);
	__list_head_link(ptr, next);
}

static inline void __list_head_del(struct list_head *ptr) {
	__list_head_link(ptr->prev, ptr->next);
}

static inline void list_del(struct list_head *ptr) {
	__list_head_del(ptr);
}

static inline void list_add(struct list_head *ptr, struct list_head *head) {
	__list_head_add(head, head->next, ptr);
}

static inline void list_add_tail(struct list_head *ptr, struct list_head *head) {
	__list_head_add(head->prev, head, ptr);
}

static inline int list_empty(struct list_head *ptr) {
	return ptr->next == ptr;
}

// Move [start, end) to head's next
static inline void list_move_range(struct list_head *start, struct list_head *end, struct list_head *head) {
	struct list_head *tail = end->prev;
	__list_head_link(start->prev, end);
	__list_head_link(tail, head->next);
	__list_head_link(head, start);
}

// get the nth item from node.
static inline struct list_head *list_item_at(struct list_head *node, int n) {
	while (n-- > 0) {
		node = node->next;
	}
	return node;
}

/**
 * 把ptr的成员全部移动到head的后面
 */
void list_move(struct list_head *ptr, struct list_head *head);
/**
 * 把ptr的成员全部移动到head的前面
 */
void list_move_tail(struct list_head *ptr, struct list_head *head);

#define list_entry(p, t, m) CONTAINER_OF(p, t, m)

#define LIST_FOR_EACH(ptr, head) \
	for (ptr = (head)->next; ptr != (head); ptr = ptr->next)

#define LIST_FOR_EACH_ENTRY(ptr, head, m) \
	for (struct list_head *__p = (head); \
		 (__p = __p->next) != (head) && (ptr = list_entry(__p, typeof(*ptr), m));)

#define LIST_FOR_EACH_ENTRY_SAFE(ptr, head, m) \
	for (struct list_head *__p = (head)->next; \
		 __p != (head) && ({ptr =list_entry(__p, typeof(*ptr), m); __p = __p->next;});)

#define LIST_FOR_EACH_ENTRY_FROM(ptr, fm, head, m) \
	for (struct list_head *__p = (fm); \
		 ptr = list_entry(__p, typeof(*ptr), m), __p != (head); \
		 __p = __p->next)

#endif // MAIN_LIST_H
