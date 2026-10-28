/*
 * Forced macro renames: a macro with both, a rewritten definition
 * and a forwarding one.
 */
static int eepr_o;
#define fr_a (eepr_o + 1)

void pu_f(void)
{
	int i = fr_a;
}

void pu_g(void)
{
	int eepr_o = 0;
	int i = fr_a;
}
