/*
 * A macro mixing attributes to be removed and to be kept gets
 * rewritten.
 */
#define __cold_noreturn __attribute__((cold, noreturn))
#define __used_aligned(x) __attribute__((used, aligned(x)))

static __cold_noreturn void espr_f(void)
{
	for (;;);
}

int espr_a __used_aligned(64) = 1;
int ee_b __used_aligned(64) = 2;

int pu_f(void)
{
	if (espr_a + ee_b)
		espr_f();
	return 0;
}
