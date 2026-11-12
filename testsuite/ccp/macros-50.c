int espr_i;
int espr_j;

static int g(int a, int b)
{
	return a + b;
}

#define LP g(
#define RP )
#define A LP espr_i,
#define B espr_j RP

int pu_f(void)
{
	return A B;
}
