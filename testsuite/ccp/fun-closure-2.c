void g(void);

void h(void)
{
	g();
}

void g(void)
{
	h();
}

void pu_f(void)
{
	g();
}
