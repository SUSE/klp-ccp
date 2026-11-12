/*
 * Objects externalized through a pointer: attributes describing the
 * original object's storage or symbol would apply to the pointer and
 * get removed, the __attribute__ as a whole if nothing remains.
 * Attributes on pointer derivations within the declarator are part of
 * the type and stay.
 */
__attribute__((section(".data..ro_after_init"))) int espr_a = 1;
int espr_b __attribute__((__aligned__(64), used)) = 2;
static int __attribute__((deprecated, section("foo"))) espr_c;
int * __attribute__((aligned(16))) espr_d;
__attribute__((used)) char espr_e[8] __attribute__((nonstring, unused));

int pu_f(void)
{
	return espr_a + espr_b + espr_c + *espr_d + espr_e[0];
}
