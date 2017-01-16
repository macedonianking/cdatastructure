#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

void reportDcheckFailure(const char *file, int lineNumber, const char *assertion) {
	if (assertion) {
		fprintf(stdout, "%s:%d DCHECK FAILURE:%s\n", file, lineNumber, assertion);
		fflush(stdout);
	} else {
		fprintf(stdout, "%s:%d DCHECK FAILURE\n", file, lineNumber);
		fflush(stdout);
	}
}

void reportDcheckBacktrace() {
	// DO NOTHING.
}
