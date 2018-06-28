/*
 * percpu.h
 *
 *  Created on: 2018年3月6日
 *      Author: lhb198710
 */

#ifndef PERCPU_H_
#define PERCPU_H_

#include "config.h"
#include "compiler.h"

#ifdef CONFIG_SMP

extern unsigned long __per_cpu_offset[NR_CPUS];

#define DEFINE_PER_CPU(type, name)  __attribute__((section(".data.percpu"))) \
    typeof(type) per_cpu__##name

#define per_cpu(var, cpu)           *RELOC_HIDE(&(per_cpu__##var), __per_cpu_offset[cpu])
#define __get_cpu_var(var)          per_cpu(var, smp_processor_id())

#define percpu_modcopy(pcpudst, src, size) \
do {    \
    unsigned int __i;   \
    for (__i = 0; __i < NR_CPUS; ++__i) {   \
        memcpy(RELOC_HIDE((pcpudst), __per_cpu_offset[__i]), (src), (size));    \
    } \
} while(0)

#else /* CONFIG_SMP */

#define DEFINE_PER_CPU(type, name)  typeof(type) per_cpu__##name
#define per_cpu(var, cpu)           (*((void)(cpu), &per_cpu__##var))
#define __get_cpu_var(var)          per_cpu__##var

#endif /* CONFIG_SMP */

#define DECLARE_PER_CPU(type, name)     extern typeof(type) per_cpu__##name
#define EXPORT_PER_CPU_SYMBOL(name)     EXPORT_SYMBOL(per_cpu__##name)
#define EXPORT_PER_CPU_SYMBOL_GPL(name) EXPORT_SYMBOL_GPL(per_cpu__##name)

#endif /* PERCPU_H_ */
