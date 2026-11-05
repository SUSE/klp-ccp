/*
 * An #include "..." from within some header, found relative to that
 * header's location, but not reachable through the search directories
 * gets retained verbatim in the output, with a warning.
 */
#include "headers-11/headers-11.h"

headers_11_t pu_f(void)
{
	return ee_a;
}

/* cp-out: warning: output line 3: header "[^"]*headers-11/headers-11-1.h" is not reachable through the search path, #include might not resolve */
