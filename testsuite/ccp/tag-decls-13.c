struct A
{
	struct B *b;
};

struct B
{};

void pu_f(struct B*)
{}


struct C
{
	struct E *e;
};

struct D
{
	struct E *e;
};

void pu_g(struct E*)
{}
