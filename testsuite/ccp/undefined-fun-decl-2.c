static inline int f(int a);

static inline int ps_g(int a)
{
	return a;
}

#include "undefined-fun-decl-2.h"

static inline int f(int a)
{
	return a;
}

int pu_h(void)
{
	return i() + ps_g(1);
}
