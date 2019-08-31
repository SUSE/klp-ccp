#define __str(x) #x
#define NO_RETAIN(x) __str(x), x

int espr_i;

void pu_f(void)
{
	NO_RETAIN(espr_i);
}
