#define G g

static void G(void)
{}

#undef G
#include "macros-15.h"

void pu_f(void)
{
	g();
	G();
}
