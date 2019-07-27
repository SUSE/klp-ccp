

#define s hello

#define __str(a) #a
#define str(a) __str(a)

void g(void)
{
	str(s);
}

#undef s

void pu_f(void)
{
	g();
	str(s);
}
