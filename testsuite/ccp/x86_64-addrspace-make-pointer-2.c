/*
 * Externalization through a pointer of an array in a named address
 * space and of a pointer object pointing into one.
 */
__seg_gs int espr_a[4];
__seg_gs int *espr_p;

int pu_f(void)
{
	return espr_a[1] + *espr_p + sizeof(espr_a);
}
