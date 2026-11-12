/*
 * Externalization of objects living in a named address space by
 * declaring them extern: the __seg_gs qualifier must be retained.
 */
static __seg_gs int ee_o = 1;
static int __seg_gs ee_p;
static __seg_fs int eer_q;

int pu_f(void)
{
	__seg_gs int *p = &ee_o;
	return ee_o + ee_p + eer_q + *p;
}
