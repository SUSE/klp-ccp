/*
 * Invocations of macros expanding to nothing: the one in the retained
 * part of DECL1's replacement list is kept, the one in the deleted
 * initializer of DECL2 is dropped together with it.
 */
#define EMPTY
#define DECL1(name, init) int name EMPTY = init
#define DECL2(name, init) int name = EMPTY init

DECL1(eep_a, 1);
DECL2(eep_b, 2);

int pu_f(void)
{
	return eep_a + eep_b;
}
