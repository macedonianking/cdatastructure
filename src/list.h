#ifndef MAIN_LIST_H
#define MAIN_LIST_H

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#include "util.h"

#define INIT_LIST_HEAD(ptr) (ptr)->next = (ptr)->prev = (ptr)
#define DEFINE_LIST_HEAD(name) \
	struct list_head name = {.next = &name, .prev = &name}

#define list_entry(ptr, type, m) CONTAINER_OF(ptr, type, m)

#define LIST_FOR_EACH_ENTRY(ptr, home, m) \
	for (struct list_head *__p = (home)->next; \
		 ptr = list_entry(__p, typeof(*ptr), m), __p != (home); \
		 __p = __p->next)

#define LIST_FOR_EACH_ENTRY_FROM(ptr, fm, head, m) \
	for (struct list_head *__p = (fm); \
		 ptr = list_entry(__p, typeof(*ptr), m), __p != (head); \
		 __p = __p->next)

#define LIST_FOR_EACH_ENTRY_SAFE(ptr, head, m) \
	for (struct list_head *__p = (head)->next; \
		 (__p != (head)) && ({ptr = list_entry(__p, typeof(*ptr), m); __p = __p->next;});)

static inline void __link_list_node(struct list_head *prev, struct list_head *next) {
	prev->next = next;
	next->prev = prev;
}

static inline void __delete_list_node(struct list_head* ptr) {
	__link_list_node(ptr->prev, ptr->next);
}

static inline void list_add(struct list_head *node, struct list_head *head) {
	__link_list_node(node, head->next);
	__link_list_node(head, node);
}

static inline void list_add_tail(struct list_head *node, struct list_head *head) {
	__link_list_node(head->prev, node);
	__link_list_node(node, head);
}

static inline int list_empty(struct list_head *node) {
	return node->next == node;
}

static inline void list_del(struct list_head *node) {
	__delete_list_node(node);
	INIT_LIST_HEAD(node);
}

#endif // MAIN_LIST_H
