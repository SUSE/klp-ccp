/*
 * The kernel's __init and friends: the section and the retpoline
 * disabling go away, the remaining attributes make the macro get
 * rewritten.
 */
#define __section(s) __attribute__((__section__(#s)))
#define __cold __attribute__((cold))
#define __noretpoline __attribute__((indirect_branch("keep")))
#define __init __section(.init.text) __cold __noretpoline
#define __exit __section(.exit.text) __cold
#define __ref __section(.ref.text) __attribute__((noinline))

static int __init f(int x)
{
	return x;
}

static int __exit g(int x)
{
	return x;
}

static int __ref h(int x)
{
	return x;
}

int __init pu_f(int x)
{
	return f(x) + g(x) + h(x);
}
