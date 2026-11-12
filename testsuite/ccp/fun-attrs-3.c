/*
 * Attributes on prototypes attach to the function as well: they get
 * removed from prototypes referring to the live patch's copy, at file
 * scope and within function bodies, but not from declarations of
 * externalized functions.
 */
static int f(int x) __attribute__((section(".init.text")));
static int __attribute__((section(".init.text"))) f(int x)
{
	return x;
}

static int g(int x);
static int g(int x) __attribute__((section(".init.text")));
static int g(int x)
{
	return x;
}

int ee_h(int x) __attribute__((section(".init.text")));

int pu_f(int x)
{
	extern int f(int x) __attribute__((section(".init.text")));
	extern int ee_h(int x) __attribute__((section(".init.text")));

	return f(x) + g(x) + ee_h(x);
}
