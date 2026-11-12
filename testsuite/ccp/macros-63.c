/*
 * X may cut through the expansion of a macro invoked from the
 * argument: espr_a is shadowed at the second reference, espr_b is
 * externalized at both. Retaining X = '+ espr_b + 0' keeps the
 * conflicting espr_a out of the argument. The argument's FOO
 * invocation gets rewritten to drop espr_a and the lowered left
 * portions of both references get their own FOO variants retaining
 * it.
 */
int espr_a;
int espr_b;

#define FOO espr_a + espr_b
#define B(x, y) (x + y)
#define A(x) (B(x, 1) + ({ int espr_a = 1; x; }))

int pu_f(void)
{
	return A(FOO + 0);
}
