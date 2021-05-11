static int g(void)
{
	return 1;
}

static int h(void)
{
	return 1;
}

static int i(void)
{
	return 1;
}

static int j(void)
{
	return 1;
}

static int k(void)
{
	return 1;
}

static int l(void)
{
	return 1;
}

static int m(void)
{
	return 1;
}

static int n(void)
{
	return 1;
}

void pu_f(int x)
{
	x ? g() : h();
	0 ? i() : j();
	1 ? k() : l();
	(m(), 0) ? : n();
}
