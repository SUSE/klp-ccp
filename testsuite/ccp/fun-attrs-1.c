/*
 * Function definitions emitted into the live patch, which has a
 * lifetime independent of the patched target: attributes placing the
 * function into a section or registering it in the constructor or
 * destructor tables get removed, as do the symbol related ones. The
 * ones describing the body's code generation or the call semantics
 * stay.
 */
static __attribute__((section(".init.text"), cold)) int f(int x)
{
	return x;
}

static int __attribute__((constructor)) g(void)
{
	return 1;
}

static int h(int x) __attribute__((destructor, noinline));

static int h(int x)
{
	return x;
}

static __attribute__((__section__(".exit.text"))) __attribute__((noinline))
int i(int x)
{
	return x;
}

int __attribute__((weak, visibility("hidden"))) j(int x)
{
	return x;
}

int pu_f(int x)
{
	return f(x) + g() + h(x) + i(x) + j(x);
}
