#define __foo_espr_i 0

#define __concat(x, y) x ## y
#define FOO(x) __concat(__foo_, x), x

int espr_i;

void pu_f(void)
{
	FOO(espr_i);
}
