/*
 * 'inline' coming from macros, like the kernel's redefinition of
 * the keyword to add attributes: the whole invocation disappears if
 * nothing of it remains, otherwise the macro gets rewritten.
 */
#define __always_inline inline __attribute__((always_inline))
#define inline inline __attribute__((unused)) __attribute__((no_instrument_function))

static __always_inline int espr_f(int x)
{
	return x;
}

static inline int espr_g(int x)
{
	return x;
}

static inline int ee_h(int x)
{
	return x;
}

int pu_f(void)
{
	return espr_f(1) + espr_g(2) + ee_h(3);
}
