/*
 * Lowering an argument reference into the argument of a rescan
 * child: B references its parameter x from the replacement list, so
 * the position gets macro expanded and the rewritten argument source
 * (KLPR_FOO) can be lowered there. The second x reference, with
 * espr_i shadowed, stays as is.
 */
int espr_i;

#define FOO espr_i
#define B(x, y) (x + y)
#define A(x) (B(x, 1) + ({ int espr_i = 1; x; }))

int pu_f(void)
{
	return A(FOO);
}
