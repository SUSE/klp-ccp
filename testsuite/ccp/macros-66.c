/*
 * Wrap edits on '##' results: the pointer dereference wraps the
 * pasted operands, (*klpe_ ## x ## y). The prefix paste also applies
 * when the concatenation's first operand is a literal token of the
 * replacement list, 'klpe_ ## espr_ ## y'.
 */
int espr_ab;
int espr_cd;

#define __concat(x, y) x ## y
#define concat(x, y) __concat(x, y)
#define PFX(y) espr_ ## y

int pu_f(void)
{
	return concat(espr_a, b) + concat(espr_c, d) + PFX(ab) + PFX(cd);
}
