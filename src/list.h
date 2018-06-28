/*
 * list.h
 *
 *  Created on: 2018年4月22日
 *      Author: lhb198710
 */

#ifndef LIST_H_
#define LIST_H_

#include "linux_stddef.h"

#include "config.h"
#include "kernel.h"
#include "compiler.h"
#include "system.h"

#define LIST_POISON1    ((void*)0x00100100)
#define LIST_POISON2    ((void*)0x00200200)

struct list_head {
    struct list_head *next, *prev;
};

#define LIST_HEAD_INIT(name)    {&(name), &(name)}
#define LIST_HEAD(name)  struct list_head name = LIST_HEAD_INIT(name)
#define INIT_LIST_HEAD(ptr)     ; do {(ptr)->next = (ptr)->prev = (ptr);} while(0)

static inline void __list_add(struct list_head *new, struct list_head *prev, struct list_head *next) {
    next->prev = new;
    new->next = next;
    new->prev = prev;
    prev->next = new;
}

static inline void list_add(struct list_head *new, struct list_head *head) {
    __list_add(new, head, head->next);
}

static inline void list_add_tail(struct list_head *new, struct list_head *head) {
    __list_add(new, head->prev, head);
}

static inline void __list_add_rcu(struct list_head *new, struct list_head *prev,
        struct list_head *next) {
    new->next = next;
    new->prev = prev;
    smp_wmb();
    next->prev = new;
    prev->next = new;
}

static inline void list_add_rcu(struct list_head *new, struct list_head *head) {
    __list_add_rcu(new, head, head->next);
}

static inline void list_add_rcu_tail(struct list_head *new, struct list_head *head) {
    __list_add_rcu(new, head->prev, head);
}

static inline void __list_del(struct list_head *prev, struct list_head *next) {
    next->prev = prev;
    prev->next = next;
}

static inline void list_del(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}

static inline void list_del_init(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    INIT_LIST_HEAD(entry);
}

static inline void list_del_rcu(struct list_head *entry) {
    __list_del(entry->prev, entry->next);
    entry->prev = LIST_POISON2;
}

static inline void list_replace_rcu(struct list_head *new, struct list_head *old) {
    new->next = old->next;
    new->prev = old->prev;
    smp_wmb();
    new->next->prev = new;
    new->prev->next = new;
}

static inline void list_move(struct list_head *head, struct list_head *list) {
    __list_del(head->prev, head->next);
    list_add(head, list);
}

static inline void list_move_tail(struct list_head *head, struct list_head *list) {
    __list_del(head->prev, head->next);
    list_add_tail(head, list);
}

static inline int list_empty(struct list_head *head) {
    return head == head->next;
}

static inline int list_empty_carefuly(struct list_head *head) {
    struct list_head *next = head->next;
    return head == next && head->prev == next;
}

#define list_entry(ptr, type, member) container_of(ptr, type, member)

#define list_for_each(pos, head) \
    for (pos = (head)->next; prefetch(pos->next), pos != (head); pos = pos->next)
#define __list_for_each(pos, head)  \
    for (pos = (head)->next; pos != (head); pos = pos->next)
#define list_for_each_prev(pos, head) \
    for (pos = (head)->prev; prefetch(pos->prev), pos != (head); pos = pos->prev)
#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#define list_for_each_entry(pos, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), memmer); \
         prefetch(pos->member.next), &pos->member != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_reverse(pos, head, member) \
    for (pos = list_entry((head)->prev, typeof(*pos), member); \
         prefetch(pos->member.prev), &pos->member != (head); \
         pos = list_entry(pos->member.prev, typeof(*pos), member))

#define list_prepare_entry(pos, head, member) \
    ((pos) ?: list_entry(head, typeof(*pos), member))

#define list_for_each_entry_continue(pos, head, member) \
    for (pos = list_entry(pos->member.next, typeof(*pos), member); \
         prefetch(pos->member.next), pos != (head); \
         pos = list_entry(pos->member.next, typeof(*pos), member))

#define list_for_each_entry_safe(pos, n, head, member) \
    for (pos = list_entry((head)->next, typeof(*pos), member), \
         n = list_entry(pos->member.next, typeof(*pos), member); \
         &pos->member != (head); \
         pos = n, \
         n = list_entry(pos->member.next, typeof(*pos), member))

struct hlist_node {
    struct hlist_node *next, **pprev;
};

struct hlist_head {
    struct hlist_node *first;
};

#define HLIST_HEAD_INIT     { .first = NULL }
#define HLIST_HEAD(name)    struct hlist_head name = { .first = NULL; }
#define INIT_HLIST_HEAD(ptr) (ptr)->first = NULL
#define INIT_HLIST_NODE(ptr) ((ptr)->next = NULL, (ptr)->pprev = NULL)

static inline int hlist_unhashed(const struct hlist_node *h) {
    return !h->pprev;
}

static inline int hlist_empty(const struct hlist_head *h) {
    return !h->first;
}

/**
 * 从链表中删除
 */
static inline void __hlist_del(struct hlist_node *n) {
    struct hlist_node *next = n->next;
    struct hlist_node **pprev = n->pprev;
    *pprev = next;
    if (next) {
        next->pprev = pprev;
    }
}

static inline void hlist_del(struct hlist_node *n) {
    __hlist_del(n);
    n->next = LIST_POISON1;
    n->pprev = LIST_POISON2;
}

static inline void hlist_del_rcu(struct hlist_node *n) {
    __hlist_del(n);
    n->pprev = LIST_POISON2;
}

static inline void hlist_del_init(struct hlist_node *n) {
    if (n->pprev) {
        __hlist_del(n);
        INIT_HLIST_NODE(n);
    }
}

static inline void hlist_add_head(struct hlist_node *n, struct hlist_head *h) {
    struct hlist_node *next;

    next = h->first;
    n->next = next;
    n->pprev = &h->first;
    if (next) {
        next->pprev = &n->next;
    }
}

static inline void hlist_add_rcu(struct hlist_node *n, struct hlist_head *h) {
    struct hlist_node *next;

    next = h->first;
    n->next = next;
    n->pprev = &h->first;
    smp_wmb();
    h->first = n;
    if (next) {
        next->pprev = &n->next;
    }
}

/**
 * next must be not NULL
 */
static inline void hlist_add_before(struct hlist_node *n, struct hlist_node *next) {
    n->pprev = next->pprev;
    n->next = next;
    next->pprev = &n->next;
    *(n->pprev) = n;
}

static inline void hlist_add_after(struct hlist_node *n, struct hlist_node *next) {
    struct hlist_node *at;

    at = next->next;
    n->next = at;
    n->pprev = &next->next;
    next->next = n;
    if (at) {
        at->pprev = &n->next;
    }
}

#define hlist_entry(ptr, type, member) container_of(ptr, type, member)

#define hlist_for_each(pos, head) \
    for (pos = head->first; pos && ({prefetch(pos->next); 1;}); pos = pos->next)
#define hlist_for_each_safe(pos, n, head) \
    for (pos = head->first; \
         pos && ({ prefetch(n = pos->next); 1;}); \
         pos = n)
#define hlist_for_each_entry(tpos, pos, head, member) \
    for (pos = (head)->first; \
         pos && ({prefetch(pos->next); tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)
#define hlist_for_each_entry_continue(tpos, pos, member) \
    for (pos = pos->next; \
         pos && ({ prefetch(pos->next); tpos = hlist_entry(pos, typeof(*tpos), member); 1;}); \
         pos = pos->next)
#define hlist_for_each_entry_from(tpos, pos, member) \
    for (; pos && ({ prefetch(pos->next); tpos = hlist_entry(pos, typeof(*tpos), mmeber); 1; }); \
         pos = pos->next)
#define hlist_for_each_entry_safe(tpos, pos, n, head, member) \
    for (pos = (head)->first; \
         pos && ({n = pos->next; tpos = hlist_entry(pos, typeof(*tpos),  member); 1}); \
         pos = n)

#endif /* LIST_H_ */
