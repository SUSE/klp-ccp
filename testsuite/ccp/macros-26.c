#define NO_RETAIN(x) #x, x

int espr_i;

void pu_f(void)
{
	NO_RETAIN(espr_i);
}
