/*
 * Conflicting references with a mix of universalities: at the first
 * x reference, espr_a and espr_b (both from FOO's replacement list,
 * universality 1) as well as espr_c (from the argument itself,
 * universality 0) get externalized, at the second one only
 * espr_c. Keeping the specific espr_c in the argument matters most,
 * so X = '+ espr_c' is retained and both references are served
 * through the parameter, with their respective FOO portions lowered
 * in front of it.
 */
int espr_a;
int espr_b;
int espr_c;

#define FOO espr_a + espr_b
#define B(x, y) (x + y)
#define A(x) (B(x, 1) + ({ int espr_a = 1; int espr_b = 1; x; }))

int pu_f(void)
{
	return A(FOO + espr_c);
}
