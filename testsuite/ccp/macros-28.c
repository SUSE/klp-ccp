#define __foo_espr_i 0

#define NO_RETAIN(x) __foo_ ## x, x

int espr_i;

void pu_f(void)
{
	NO_RETAIN(espr_i);
}
