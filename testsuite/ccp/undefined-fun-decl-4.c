#define __gnu_inline __attribute__((__gnu_inline__))
#define inline inline __gnu_inline __attribute__((unused))

static inline int f(int a)
{
	return a;
}

__attribute__((cold)) static inline int g(int a)
{
	return a;
}

#include "undefined-fun-decl-4.h"

int pu_h(void)
{
	return i();
}
