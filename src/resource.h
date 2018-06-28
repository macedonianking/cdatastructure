/*
 * resource.h
 *
 *  Created on: 2018年5月25日
 *      Author: lhb198710
 */

#ifndef RESOURCE_H_
#define RESOURCE_H_

#define RLIMIT_CPU      0
#define RLIMIT_AS       9
#define RLIM_NLIMITS    13

#ifndef RLIM_INFINITY
#define RLIM_INFINITY   ~0UL
#endif

struct rlimit {
    unsigned long rlim_cur;
    unsigned long rlim_max;
};

#endif /* RESOURCE_H_ */
