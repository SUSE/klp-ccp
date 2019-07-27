void ee_g(void);

void h(void)
{
	ee_g();
}

void ee_g(void)
{
	h();
}

void pu_f(void)
{
	ee_g();
}
