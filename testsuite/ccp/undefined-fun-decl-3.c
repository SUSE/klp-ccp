static inline __attribute__((gnu_inline)) int f(int a)
{
	return a;
}

__attribute__((__gnu_inline__)) static inline int g(int a)
{
	return a;
}

static __attribute__((cold, gnu_inline)) inline int k(int a)
{
	return a;
}

static inline int l(int a) __attribute__((unused, gnu_inline));

static inline int l(int a)
{
	return a;
}

extern inline __attribute__((gnu_inline)) int m(int a)
{
	return a;
}

#include "undefined-fun-decl-3.h"

int pu_h(void)
{
	return i();
}
