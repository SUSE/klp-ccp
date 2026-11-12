/*
 * Forced macro renames: invocations of the macros listed by the
 * policy get renamed even though no edit affects them. Macros not
 * needing any rewrite otherwise get a definition forwarding to the
 * original, macros invoking a renamed macro get rewritten.
 */
#define FR_A 1
#define fr_a(x) ((x) + 1)
#define b fr_a(2)
#define c(x) x

void pu_f(void)
{
	int i = FR_A;
	i += fr_a(i);
	i += b;
	i += c(fr_a(i));
}
