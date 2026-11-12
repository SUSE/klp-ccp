/*
 * Forced renames of functions: a function getting rewritten and
 * renamed anyway doesn't need a forwarding macro.
 */
static int eepr_o;

static int fr_a(int x)
{
	return eepr_o + x;
}

void pu_f(void)
{
	int i = fr_a(1);
}
