/*
 * Externalization by extern declaration of objects whose named
 * address space comes through a typedef or a typeof().
 */
typedef __seg_gs int gs_int;
static gs_int ee_o;
static typeof(ee_o) ee_p;
static const gs_int ee_q = 1;

int pu_f(void)
{
	return ee_o + ee_p + ee_q;
}
