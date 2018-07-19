/*
 * siginfo.h
 *
 *  Created on: 2018年7月9日
 *      Author: lhb
 */

#ifndef SIGINFO_H_
#define SIGINFO_H_

typedef struct siginfo {
    int si_signo;
    int si_errno;
    int si_code;

    union {
        struct {
            void *si_addr;
            int _trapno;
        } _segfault;
    };
} siginfo_t;

#endif /* SIGINFO_H_ */
