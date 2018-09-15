/*
 * init.h
 *
 *  Created on: 2018年4月6日
 *      Author: lhb198710
 */

#ifndef INIT_H_
#define INIT_H_

#ifdef __MACH__
#define __init __attribute__((section("TEXT,init")))
#else
#define __init __attribute__((section(".init.text")))
#endif

#endif /* INIT_H_ */
