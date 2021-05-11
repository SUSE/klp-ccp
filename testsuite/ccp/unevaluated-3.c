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

void pu_f(void)
{
	__builtin_choose_expr(0, i(), j());
	__builtin_choose_expr(1, k(), l());
}
