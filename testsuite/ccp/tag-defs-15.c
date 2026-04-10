struct a
{
	char b;
	int s;
};

struct ab
{
	struct a;
	char d[74];
};

void *pu_f(void)
{
	struct a b;
	struct ab test;
}
