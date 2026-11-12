/*
 * A replaced initializer of a local static forces a rewrite of the
 * enclosing function.
 */
#include "obj-init-replacement-4.h"

void pu_f(void)
{
	g();
}
/* cp-out: warning: [^:]*: line 3, column 12: referenced object "ri_x" cannot get externalized */
