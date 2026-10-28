/*
 * Forced macro renames: invocations expanding to nothing.
 */
#define fr_a(x)
#define FR_B

void pu_f(void)
{
	int FR_B i = 1;
	fr_a(i);
}
