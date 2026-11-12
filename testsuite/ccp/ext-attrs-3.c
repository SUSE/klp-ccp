/*
 * Functions externalized through a pointer: attributes GCC applies
 * to function types, like calling convention and call semantics ones,
 * carry over to the pointed-to type and stay, declaration-only ones
 * get removed.
 */
static __attribute__((cold, noreturn)) void espr_f(void)
{
	for (;;);
}

static int __attribute__((const, noinline)) espr_g(int x)
{
	return x;
}

static void espr_h(void) __attribute__((section(".init.text"), used));

static void espr_h(void)
{
}

static int __attribute__((__noinline__, format(printf, 1, 2)))
espr_i(const char *fmt, ...)
{
	return 0;
}

int pu_f(void)
{
	espr_h();
	espr_i("%d", 1);
	if (espr_g(1))
		espr_f();
	return espr_g(2);
}
