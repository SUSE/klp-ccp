/*
 * Attributes from macros: removing all of a macro's attributes
 * drops the invocation as a whole, kept ones leave the invocation
 * untouched, so that the original macro definition can be reused.
 */
#define __ro_after_init __attribute__((section(".data..ro_after_init")))
#define __cold __attribute__((cold))
#define __aligned(x) __attribute__((aligned(x)))

int espr_a __ro_after_init = 1;
int espr_b __ro_after_init __aligned(64) = 2;
int ee_c __ro_after_init = 3;
int ee_d __ro_after_init __aligned(64) = 4;

static __cold void espr_f(void)
{
}

static __cold void ee_g(void)
{
}

int pu_f(void)
{
	espr_f();
	ee_g();
	return espr_a + espr_b + ee_c + ee_d;
}
