static inline int f(int a)
{
	return a;
}

static int g(int a)
{
	return a;
}

inline static int k(int a)
{
	return a;
}

#include "undefined-fun-decl-1.h"

int pu_h(void)
{
	return i();
}
