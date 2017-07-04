#ifndef APUE_SRC_CHAPTER11_H
#define APUE_SRC_CHAPTER11_H

int chapter11_main(int argc, char **argv);

/**
 * pthread_equal(),
 * pthread_self()
 */
void chapter11_3(int argc, char **argv);
void chapter11_4(int argc, char **argv);
void chapter11_6(int argc, char **argv);
void chapter11_7(int argc, char **argv);

void chapter11_6_2(int argc, char **argv);
void chapter11_6_6(int argc, char **argv);
/**
 * Demonstrate how to use pthread_spinlock_t
 */
void chapter11_6_7(int argc, char **argv);
/**
 * Demonstrate how to use barrier, a little like CountDownLatch.
 */
void chapter11_6_8(int argc, char **argv);

#endif
