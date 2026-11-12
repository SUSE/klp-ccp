/*
 * Pure rename implemented through a '##' paste: the call to the
 * patched function gets renamed to klpp_pu_g, whereas the local
 * variable of the same name at the second reference stays.
 */
void pu_g(void)
{
}

#define A(f) (f(), ({ int f = 1; f; }))

int pu_f(void)
{
	return A(pu_g);
}
