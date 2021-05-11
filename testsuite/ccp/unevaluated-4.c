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

void pu_f(int x)
{
	0 || g();
	1 || h();
	x || i();

	0 && j();
	1 && k();
	x && l();
}
