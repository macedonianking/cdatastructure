/*
 * linkage.h
 *
 *  Created on: 2018年4月6日
 *      Author: lhb198710
 */

#ifndef LINKAGE_H_
#define LINKAGE_H_

#define asmlinkage __attribute__((regparm(0)))
#define fastcall __attribute__((regparm(3)))
#define FASTCALL(x) x fastcall

#endif /* LINKAGE_H_ */
