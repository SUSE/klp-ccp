#define MULTI(x)			\
	do {				\
		x = 1;			\
		espr_j = 2;		\
	} while (0)

int espr_i;
int espr_j;

void pu_f(void)
{
	MULTI(espr_i);
}
