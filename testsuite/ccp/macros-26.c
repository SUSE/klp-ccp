#define FOO(x) #x, x

int espr_i;

void pu_f(void)
{
	FOO(espr_i);
}
