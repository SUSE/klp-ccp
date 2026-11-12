/*
 * __has_attribute() invocations within a rewritten macro's
 * replacement list get carried over unmodified, including their
 * operands, whether referring to a parameter or expanding from
 * another macro.
 */
#define ATTR __unused__
#define CHECK(a) (__has_attribute(a) + __has_attribute(ATTR) + pu_g())

int pu_g(void)
{
	return 1;
}

int pu_f(void)
{
	return CHECK(unused) + CHECK(ATTR) + CHECK(nonexistent);
}
