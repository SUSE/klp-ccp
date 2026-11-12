/*
 * Externalization through a pointer of objects whose named address
 * space comes through a typedef or a typeof().
 */
typedef __seg_gs int gs_int;
gs_int espr_o;
typeof(espr_o) espr_p;
const gs_int espr_q;

int pu_f(void)
{
	return espr_o + espr_p + espr_q;
}
