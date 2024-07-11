_Static_assert(sizeof(struct A { int a; }) != 0, "");

void pu_f(struct A)
{}
