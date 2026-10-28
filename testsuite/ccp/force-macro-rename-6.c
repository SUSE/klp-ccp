/*
 * Forced macro renames: multiple definitions of the same macro name
 * and an invocation whose name token comes from another macro's
 * expansion.
 */
#define fr_a(x) (x)
#define b fr_a

void pu_f(void)
{
	int i = b(1);
}

#undef fr_a
#define fr_a(x) (x + 1)

void pu_g(void)
{
	int i = fr_a(2);
}
