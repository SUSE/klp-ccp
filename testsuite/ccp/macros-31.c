int espr_o = 0;

#define A(x) ++x

#define B(x) A(x)+A(x)

#define C(x) B(x)+B(x)

#define D(x) C(x)+C(x)

#define E(x) D(x)+D(x)

#define F(x) E(x)+E(x)

#define G(x) F(x)+F(x)


void pu_f(void)
{
	G(espr_o);
}
