/*
 * Forced macro renames: an object definition from a header invoking
 * a renamed macro cannot get duplicated, the header becomes
 * uneligible for inclusion.
 */
#include "force-macro-rename-4.h"

void pu_f(void)
{
	int i = so + h();
}
/* cp-out: warning: [^:]*: line 3, column 17: referenced object "so" cannot get externalized */
