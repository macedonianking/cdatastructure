#include "macros.h"

#include <stdio.h>
#include <stdlib.h>

void reportDcheckFailure(const char *file, int lineNumber, const char *function, const char *assertion) {
	if (assertion) {
		fprintf(stdout, "%s:%s[%d] DCHECK FAILURE:%s\n", file, function, lineNumber, assertion);
		fflush(stdout);
	} else {
		fprintf(stdout, "%s:%s[%d] DCHECK FAILURE\n", file, function, lineNumber);
		fflush(stdout);
	}
}

void reportDcheckBacktrace() {
	// DO NOTHING.
}
