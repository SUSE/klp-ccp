int espr_i;
int espr_j;

static int g(int a, int b)
{
	return a + b;
}

#define A g(espr_i,
#define B espr_j)
#define TOP A B

int pu_f(void)
{
	return TOP;
}
