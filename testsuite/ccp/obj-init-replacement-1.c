/*
 * Initializers of non-externalized objects at file scope replaced
 * by the policy: the initializers' dependencies, including
 * externalizations, are retained.
 */
static int eepr_o;

static int eepr_f(void)
{
	return 1;
}

static void *ri_p = (void *)&eepr_o;
static int (*ri_f)(void) = eepr_f;
static int *keep_p = &eepr_o;

void pu_f(void)
{
	ri_p = keep_p;
	ri_f();
}
/* cp-out: warning: [^:]*: line 13, columns 12-17: referenced object "ri_p" cannot get externalized */
/* cp-out: warning: [^:]*: line 15, columns 11-18: referenced object "keep_p" cannot get externalized */
/* cp-out: warning: [^:]*: line 14, columns 11-24: referenced object "ri_f" cannot get externalized */
