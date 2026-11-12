/*
 * #includes extending into some external declaration from the
 * outside, like those placed within a function body or a struct
 * definition, must not cause the enclosing external declaration to
 * get emitted if not needed otherwise.
 */
struct ops { int a; int b; };
struct ops the_ops;

#define OP(x) int ee_##x;
#include "headers-10.h"

static inline void g(struct ops *o)
{
	the_ops = *o;
#define OP(x) ee_##x = the_ops.x;
#include "headers-10.h"
}

void h(struct ops *o)
{
	g(o);
}

struct s {
#include "headers-10-1.h"
};

struct u {
#include "headers-10-1.h"
};

int pu_f(struct s *p)
{
	return ee_a + p->a;
}
