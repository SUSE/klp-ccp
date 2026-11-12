/*
 * An edit on the identifier resulting from a '##' concatenation is
 * implemented by pasting onto the operands rather than by placing the
 * edited identifier into the replacement list verbatim: 'klpe_ ## x
 * ## y' yields the renamed identifier for any pair of arguments, so
 * both invocations share a single rewritten definition of __concat,
 * and hence of concat.
 */
int eer_ab;
int eer_cd;

#define __concat(x, y) x ## y
#define concat(x, y) __concat(x, y)

int pu_f(void)
{
	return concat(eer_a, b) + concat(eer_c, d);
}
