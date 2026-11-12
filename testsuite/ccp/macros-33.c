#define DEFINE_FOO(name) int name = 0

DEFINE_FOO(espr_o);

void pu_f(void)
{
	espr_o = 1;
}
