/*
 * The first x reference would get lowered into B's argument (c.f.
 * macros-53.c), but as the argument's expansion invokes __COUNTER__,
 * it must not get expanded a second time. The first reference gets
 * replaced by its edited expansion result verbatim instead and so
 * does the second one, with espr_i shadowed, which could have been
 * served through the parameter otherwise: the argument is not to get
 * expanded at all anymore.
 */
int espr_i;

#define FOO espr_i
#define B(x, y) (x + y)
#define A(x) (B(x, 1) + ({ int espr_i = 1; x; }))

int pu_f(void)
{
	return A(FOO + __COUNTER__);
}
