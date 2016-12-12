#include "chapter10.h"

#include <time.h>

struct dial_bill {
	struct tm tm_start, tm_end;
	char fm_no[12];
	char to_no[12];
	char exchange_no[12];
};
