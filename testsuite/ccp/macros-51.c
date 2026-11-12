/*
 * A closing parenthesis emerging from a macro expansion cannot
 * terminate an argument list opened outside of it: the unexpanded B
 * gets collected as part of the argument and the invocation runs
 * into EOF. That's invalid and must get diagnosed rather than
 * crashing.
 */
int i;
int j;

static int g(int a, int b)
{
	return a + b;
}

#define B j)
#define M(a, b) g(a, b)

int pu_f(void)
{
	return M(i, B;
}
/* cp-out: error: [^:]*: line 24, column 0: no closing right parenthesis in macro invocation */
