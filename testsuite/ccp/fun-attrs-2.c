/*
 * The kernel disables retpolines for init-only code: the "keep"
 * variants of the indirect_branch and function_return attributes
 * must not carry over to the live patch's copy, others are left
 * alone. visibility and externally_visible get removed only if the
 * copy ends up with internal linkage.
 */
static __attribute__((indirect_branch("keep"))) int f(int x)
{
	return x;
}

static __attribute__((function_return("keep"), indirect_branch("thunk")))
int g(int x)
{
	return x;
}

static __attribute__((externally_visible)) int h(int x)
{
	return x;
}

__attribute__((externally_visible)) int i(int x)
{
	return x;
}

__attribute__((externally_visible, visibility("default"))) int pe_f(int x)
{
	return f(x) + g(x) + h(x) + i(x);
}

__attribute__((externally_visible)) int ps_g(int x)
{
	return x;
}
