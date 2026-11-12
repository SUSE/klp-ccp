/*
 * Three references, two of them agreeing on externalizing espr_i
 * (through FOO) and one keeping it (shadowed): serving the two
 * agreeing ones through the parameter and lowering the third costs
 * less than the other way around.
 */
int espr_i;

#define FOO espr_i
#define A(x) (x + x + ({ int espr_i = 1; x; }))

int pu_f(void)
{
	return A(FOO);
}
