#define void() int

static void() g(void)
{}

#include "macros-23.h"

void pu_f(void)
{
	g();
	h();
}
