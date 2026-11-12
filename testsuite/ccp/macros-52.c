/*
 * Lowering an argument reference at a rescan child's name position:
 * the first x reference becomes the name of the B invocation and
 * gets edited through B's rename, while the second one, with espr_i
 * shadowed, stays as is. The reference can't be served through the
 * parameter anymore. As the name token is subject to rescan, the
 * rewritten argument source (KLPR_B) gets lowered there.
 */
int espr_i;

#define B(a, b) (espr_i + a + b)
#define A(x) (x(1, 2) + ({ int espr_i = 1; x(3, 4); }))

int pu_f(void)
{
	return A(B);
}
