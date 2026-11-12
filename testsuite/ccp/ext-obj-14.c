/*
 * Objects externalized without renaming whose linkage gets changed
 * from external to internal: their linkage can be made internal by
 * means of an extra 'static' declaration preceeding an 'extern' one
 * from a header, but not one without any storage class specifier.
 */
#include "ext-obj-14.h"
#include "ext-obj-14-1.h"

int pu_f(void)
{
	return es_a + es_b;
}
