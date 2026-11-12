/*
 * B's two x references conflict: espr_i is shadowed at the first,
 * espr_j at the second one. Serving the first reference through the
 * parameter and lowering the second one costs less argument material
 * in B's definition than lowering both, so B's argument gets espr_j
 * and espr_k externalized and the second reference gets its edited
 * expansion result in place of x. Propagated to A, its first x
 * reference (inside B's argument) now has espr_j and espr_k
 * externalized, the second one espr_i and espr_j (espr_k is
 * shadowed). Again, serving the first reference and lowering the
 * second one costs less than retaining only the agreed upon '+ BAZ +'
 * in the argument.
 */
int espr_i;
int espr_j;
int espr_k;

#define FOO espr_i
#define BAZ espr_j
#define QUX espr_k
#define B(x) (({ int espr_i = 1; x; }) + ({ int espr_j = 1; x; }))
#define A(x) (B(x) + ({ int espr_k = 1; x; }))

int pu_f(void)
{
	return A(FOO + BAZ + QUX);
}
