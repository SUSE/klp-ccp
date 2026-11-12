#define SIG(n) extern void n(void)

SIG(g)
{
}

#include "macros-48.h"

void pu_f(void)
{
	g();
	h();
}
/* cp-out: warning: [^:]*: line 12, column 1: function "h" without definition cannot get externalized */
