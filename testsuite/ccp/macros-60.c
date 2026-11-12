/*
 * Like macros-59.c, but the conflict is on the specific espr_c
 * (universality 0) only, which is shadowed at the second
 * reference. Excluding it from X would cost the specific identifier
 * at both references, so X is the whole argument: the first
 * reference is served and the second one lowered.
 */
int espr_a;
int espr_b;
int espr_c;

#define FOO espr_a + espr_b
#define B(x, y) (x + y)
#define A(x) (B(x, 1) + ({ int espr_c = 1; x; }))

int pu_f(void)
{
	return A(FOO + espr_c);
}
