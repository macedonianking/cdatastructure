#include "chapter2.h"

#include <stdio.h>
#include <stdlib.h>

#include "int_limits.h"

void chapter2_main(int argc, char **argv) {
    chapter2_excecise_1();
}

void chapter2_excecise_1() {
    printf("min=%u, max=%u\n", UINT_MIN_V, UINT_MAX_V);
}
