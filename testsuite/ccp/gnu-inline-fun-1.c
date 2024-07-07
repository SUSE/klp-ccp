extern inline __attribute__((gnu_inline, always_inline)) void g(void)
{}

extern inline __attribute__((gnu_inline)) void h(void)
{}

void pu_f(void)
{
	g();
	h();
}
