#include "chapter4.h"

#include <stdio.h>
#include <stdlib.h>

#include "string_ext.h"

void chapter4_14_7_problem() {
	char buffer[] = " dldldld aldldl     aldldlft\t ddlaldldf";
	string_deblank(buffer);
	printf("%s\n", buffer);
}
