/*
 * Initializers replaced from within a macro expansion: all rewritten
 * macro definitions become identical and get deduplicated. The
 * externalizations referenced from the original initializers are
 * retained.
 */
#define ADDRESSABLE(sym)					\
	static void * __attribute__((__used__))			\
		ri_##sym = (void *)&sym;

static int eepr_a;
static int eepr_b;

void pu_f(void)
{
	ADDRESSABLE(eepr_a)
	ADDRESSABLE(eepr_b)
}
/* cp-out: warning: [^:]*: line 16, columns 1-20: referenced object "ri_eepr_a" cannot get externalized */
/* cp-out: warning: [^:]*: line 17, columns 1-20: referenced object "ri_eepr_b" cannot get externalized */
