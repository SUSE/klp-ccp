/*
 * Like macros-57.c, but 'lb' is referenced twice and the second
 * reference, in the dropped second declarator, doesn't have the
 * length insertion at its tail. Moving the insertion into 'lb' is
 * thus not feasible, it goes to the head of 'rest' instead.
 */
#define ARR2(name, lb, rest) char name lb rest, name##_other lb 1]

ARR2(espr_o, [, ] = "123");

void pu_f(void)
{
	char s[sizeof(espr_o)];
}
