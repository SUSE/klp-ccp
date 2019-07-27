#define klpp_pu_f hello

#define __str(a) #a
#define str(a) __str(a)

static void g(void)
{
	str(klpp_pu_f);
}

void pu_f(void)
{
	g();
	str(klpp_pu_f);
}
