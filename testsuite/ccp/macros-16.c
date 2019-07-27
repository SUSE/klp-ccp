#define G g

static void G(void)
{}

#undef G
#include "macros-16.h"

void pu_f(void)
{
	g();
	h();
}
