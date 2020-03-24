#include "runner.h"
#include "check_clogs.h"
#include <stdlib.h>

int main()
{
	int failed_count = run_suite_forkless(clogs_suite());
	return (failed_count == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
