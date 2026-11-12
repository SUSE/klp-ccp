#define __foo_espr_i 0

#define FOO(x) __foo_ ## x, x

int espr_i;

void pu_f(void)
{
	FOO(espr_i);
}
