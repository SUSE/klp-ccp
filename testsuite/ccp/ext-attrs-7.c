/*
 * Externalized objects and functions declared within a function
 * body.
 */
int espr_a __attribute__((aligned(64))) = 1;
int ee_b __attribute__((aligned(64))) = 2;
void espr_f(void) __attribute__((cold));
void ee_g(void) __attribute__((cold));

int pu_f(void)
{
	extern int espr_a __attribute__((aligned(64)));
	extern int ee_b __attribute__((aligned(64)));
	extern void espr_f(void) __attribute__((cold));
	extern void ee_g(void) __attribute__((cold));

	espr_f();
	ee_g();
	return espr_a + ee_b;
}
