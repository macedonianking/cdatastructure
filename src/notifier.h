/*
 * notifier.h
 *
 *  Created on: 2018年7月16日
 *      Author: lhb
 */

#ifndef NOTIFIER_H_
#define NOTIFIER_H_

struct notifier_block {
    int (*notifier_call)(struct notifier_block*, unsigned long, void*);
    struct notifier_block *next;
    int priority;
};

extern int notifier_block_register(struct notifier_block **list, struct notifier_block *n);
extern int notifier_block_unregister(struct notifier_block **list, struct notifier_block *n);

#endif /* NOTIFIER_H_ */
