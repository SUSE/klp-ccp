/*
 * Replaced initializer completing an array type: the length gets
 * retained.
 */
static int eepr_a;
static int eepr_b;

static int *ri_arr[] = { &eepr_a, &eepr_b };

void pu_f(void)
{
	ri_arr[0] = 0;
}
/* cp-out: warning: [^:]*: line 8, columns 11-20: referenced object "ri_arr" cannot get externalized */
