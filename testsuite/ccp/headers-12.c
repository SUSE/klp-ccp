/*
 * An #include "..." from within some header, found relative to that
 * header's location, must get its header name adjusted such that it
 * resolves to the same file through the search directories when
 * retained in the output. This applies even if the original header
 * name resolves to some other file through the search directories.
 */
/* cp-include-dir: headers-12-inc */
#include <sub/headers-12.h>

headers_12_1_t pu_f(headers_12_2_t a, headers_12_3_t b)
{
	return ee_a + a + b;
}
