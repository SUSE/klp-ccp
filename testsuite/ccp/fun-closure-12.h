/*
 * The toy policy prefers not to externalize function definitions from
 * includeable headers, if possible.
 */

int a;

void h(void);

static void k(void)
{}

void ee_g(void)
{
	void (*fp)(void) = &k;

	a;
	h();
	fp();
}
