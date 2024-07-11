struct A {
	union {
		_Static_assert(sizeof(struct B { int b; }) != 0, "");
	};
};

void pu_f(struct B)
{}
