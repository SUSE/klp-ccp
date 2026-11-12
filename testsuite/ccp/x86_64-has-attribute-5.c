/*
 * Scoped attribute names are not recognized by the C frontend's
 * __has_attribute() before GCC 11.
 */
#if __has_attribute(gnu::unused)
#endif

int pu_f(void)
{
	return 0;
}
/* cp-out: error: [^:]*: line 5, columns 4-32: missing '\)' after "__has_attribute" */
