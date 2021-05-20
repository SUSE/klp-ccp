enum A
{
	a,
};

struct B
{
	enum A a;
};

enum C
{
	c,
};

struct D
{
	enum C c : 32;
};

void pu_f(struct B)
{}

void pu_g(struct D)
{}
