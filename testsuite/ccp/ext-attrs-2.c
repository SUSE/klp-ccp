/*
 * Objects externalized by extern redeclaration: attributes
 * describing the definition's storage are ineffective on a declaration
 * and kept, those altering the reference's semantics or turning the
 * declaration into a definition are removed.
 */
static __attribute__((section(".data..ro_after_init"))) int ee_a = 1;
int ee_b __attribute__((weak, aligned(64))) = 2;
int __attribute__((visibility("hidden"))) eer_c;
char ee_d[8] __attribute__((used, retain, nonstring)) = "abcdefg";

int pu_f(void)
{
	return ee_a + ee_b + eer_c + ee_d[0];
}
