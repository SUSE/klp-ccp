#define TWICE(x) x; x

int espr_i;
int espr_j;

void pu_f(void)
{
	TWICE(espr_i = espr_j);
}
