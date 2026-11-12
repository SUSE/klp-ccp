/*
 * Forced renames of functions rather than macros: references not
 * renamed otherwise get replaced by a macro forwarding to the
 * function, the function itself stays untouched.
 */
#include "force-macro-rename-8.h"

#define CALL(f) f(3)

static int fr_c(int x)
{
	return x;
}

void pu_f(void)
{
	int i = fr_a(1);
	i += fr_b(i);
	i += fr_c(i);
	i += CALL(fr_c);
}
/* cp-out: warning: [^:]*: line 18, column 6: function "fr_b" without definition cannot get externalized */
