#ifndef APUE_SRC_CHAPTER12_CHAPTER12_H
#define APUE_SRC_CHAPTER12_CHAPTER12_H

/**
 * pthread_sigmask
 */

int chapter12_main(int argc, char **argv);

/**
 * pthread_attr_t demonstration.
 */
void chapter12_3(int argc, char **argv);
/**
 * pthread_mutexattr_t
 */
void chapter12_4(int argc, char **argv);
/**
 * Reentrancy of functions.
 */
void chapter12_5(int argc, char **argv);
/**
 * Thread specified data.
 */
void chapter12_6(int argc, char **argv);
/**
 * Demonstrate how to use pthread_once_t
 */
void chapter12_6_2(int argc, char **argv);

/**
 * Demonstrate how to use pthread_setmask and sigwait.
 */
void chapter12_8(int argc, char **argv);
void chapter12_8_1(int argc, char **argv);

#endif
