#define A
#define B(a) # a
#define C(a) B(a)

void pu_f(void)
{
	C(A);
}
