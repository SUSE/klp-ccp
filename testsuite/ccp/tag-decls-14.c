struct A
{
	enum B *b;
};

enum B
{
	b,
};

void pu_f(enum B*)
{}


struct C
{
	enum E *e;
};

struct D
{
	enum E *e;
};

void pu_g(enum E*)
{}
