/*
 * threads.h
 *
 *  Created on: 2018年4月30日
 *      Author: lhb198710
 */

#ifndef THREADS_H_
#define THREADS_H_

#include "config.h"

#define PID_MAX_DEFAULT     0x8000
#define PID_MAX_LIMIT       (sizeof(long) > 4 ? 4 * 1024 * 1024 : PID_MAX_DEFAULT)

#endif /* THREADS_H_ */
