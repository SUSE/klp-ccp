/*
 * Both x references have residual edits (espr_i resp. espr_j is
 * shadowed at one of them each) and none is served through the
 * parameter. As the argument's expansion invokes __COUNTER__, it
 * can't get lowered: both references get replaced verbatim and the
 * __COUNTER__ invocation is dropped, which is permitted.
 */
int espr_i;
int espr_j;

#define FOO espr_i
#define BAZ espr_j
#define B(x, y) (x + y)
#define A(x) (({ int espr_i = 1; B(x, 1); }) + ({ int espr_j = 1; B(x, 2); }))

int pu_f(void)
{
	return A(FOO + BAZ + __COUNTER__);
}
