/*
 * Externalization by extern declaration of an array in a named
 * address space and of a pointer object pointing into one.
 */
static __seg_gs int ee_a[4] = { 1, 2, 3, 4 };
static __seg_gs int *ee_p;

int pu_f(void)
{
	return ee_a[1] + *ee_p + sizeof(ee_a);
}
