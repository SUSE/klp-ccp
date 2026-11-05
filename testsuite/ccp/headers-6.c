static
#include "headers-6-internal.h"

void pu_f(void)
{
	g();
}

/* cp-out: warning: output line 5: header "[^"]*headers-6-1.h" is not reachable through the search path, #include might not resolve */
/* cp-out: warning: output line 7: header "[^"]*headers-6-2.h" is not reachable through the search path, #include might not resolve */
