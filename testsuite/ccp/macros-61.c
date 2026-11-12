/*
 * The first reference's edit, espr_i -> (*klpe_espr_i), is
 * implemented by wrapping the parameter reference, with a '##' paste
 * for the rename, so that the identifier stays in the argument. The
 * '+ 1' outside of X gets placed next to both parameter
 * references. The rewritten definition is reusable for the second
 * invocation.
 */
int espr_i;
int espr_j;

#define A(x) (({ x; }) + ({ int espr_i = 1; int espr_j = 1; x; }))

int pu_f(void)
{
	return A(espr_i + 1) + A(espr_j + 1);
}
