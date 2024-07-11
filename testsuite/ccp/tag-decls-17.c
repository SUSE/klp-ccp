struct A {
	union {
		_Static_assert(sizeof(struct B *) != 0, "");
	};
};

void pu_f(struct B *)
{}
