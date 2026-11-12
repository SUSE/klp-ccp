/*
 * Functions externalized by extern redeclaration: attributes
 * describing the body are ineffective on a declaration and kept, the
 * ones which would require the body or alter the reference's semantics
 * get removed.
 */
static __attribute__((cold)) void ee_f(void)
{
}

static inline __attribute__((always_inline)) int ee_g(int x)
{
	return x;
}

void __attribute__((weak, section("x"))) eer_h(void)
{
}

int __attribute__((noinline, visibility("hidden"))) ee_i(int x)
{
	return x;
}

int pu_f(void)
{
	ee_f();
	eer_h();
	return ee_g(1) + ee_i(2);
}
