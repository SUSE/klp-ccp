/*
 * Objects externalized without renaming whose resulting linkage is
 * internal can have their 'static' declarations retained from an
 * #included header.
 */
#include "ext-obj-13.h"

int pu_f(void)
{
	return es_a + eu_b;
}
