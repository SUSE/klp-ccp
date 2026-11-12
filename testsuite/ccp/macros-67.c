/*
 * The init parameter's only reference gets deleted completely, the
 * initializer being dropped for the externalized object. Rather than
 * emptying the argument, which would have rewritten FOO's invocation
 * into a definition expanding to nothing, the reference itself is
 * removed from the replacement list: the argument is not expanded
 * anymore and stays as is in the invocation's source, FOO doesn't need
 * to get defined.
 */
#define FOO 1
#define DECL(name, init) int name = init

DECL(eep_a, FOO);

int pu_f(void)
{
	return eep_a;
}
