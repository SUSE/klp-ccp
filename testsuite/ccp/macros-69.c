/*
 * The second p reference is consumed as an argument by SPARSE, which
 * never references that parameter: its tokens never make it to the
 * output and thus carry no edit, in contrast to the first reference,
 * whose espr_a gets externalized. Rather than treating the two as
 * conflicting, which would force a rewritten CHECK with the second
 * reference's original tokens placed verbatim, the second one is a
 * "don't care" and the edit gets pushed down into the argument: CHECK
 * is used as is.
 */
#define SPARSE(p, space)
#define CHECK(p) ({ typeof(*p) *q__ = p; SPARSE(p, x); q__; })

int *espr_a;

int pu_f(void)
{
	return *CHECK(espr_a);
}
