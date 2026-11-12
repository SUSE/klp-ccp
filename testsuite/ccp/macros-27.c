#define __str(x) #x
#define FOO(x) __str(x), x

int espr_i;

void pu_f(void)
{
	FOO(espr_i);
}
