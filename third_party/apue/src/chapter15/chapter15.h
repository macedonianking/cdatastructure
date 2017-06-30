#ifndef APUE_CHAPTER15_CHAPTER15_H
#define APUE_CHAPTER15_CHAPTER15_H

/**
 * chapter15 main enterance.
 */
int chapter15_main(int argc, char **argv);

/**
 * Demonstrate how to use pipe.
 */
void chapter15_2(int argc, char **argv);
/**
 * Demonstrate to use pipe in same process in two threads.
 */
void chapter15_2_1(int argc, char **argv);
/**
 * Demonstrate to use pipe to send data to child process.
 */
void chapter15_2_2(int argc, char **argv);
void chapter15_2_3(int argc, char **argv);

void chapter15_5(int argc, char **argv);
void chapter15_5_1(int argc, char **argv);
void chapter15_5_2(int argc, char **argv);

#endif
