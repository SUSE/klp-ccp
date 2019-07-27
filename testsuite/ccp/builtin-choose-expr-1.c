void g(void)
{}

void h(void)
{}

void pu_f(void)
{
	__builtin_choose_expr(0, g, h)();
}
