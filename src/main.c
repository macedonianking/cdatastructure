/*
 * main.c
 *
 *  Created on: 2018年3月14日
 *      Author: lhb198710
 */

#include <stdio.h>
#include <stdlib.h>

#include "hello_static.h"
#include "hello_shared.h"

int main(int argc, char **argv) {
    printf("Hello world!!!\n");
    hello_shared();
    hello_static();
    return 0;
}

