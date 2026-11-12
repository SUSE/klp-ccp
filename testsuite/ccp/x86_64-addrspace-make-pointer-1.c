/*
 * Externalization of objects living in a named address space through
 * a pointer: the __seg_gs qualifier must be retained on the pointed-to
 * type, so that the accesses through the pointer stay %gs-relative.
 */
__seg_gs int espr_o;
int __seg_gs espr_p;
static __seg_fs int espr_q = 1;

int pu_f(void)
{
	__seg_gs int *p = &espr_o;
	return espr_o + espr_p + espr_q + *p;
}
