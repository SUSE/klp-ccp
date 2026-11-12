/*
 * Forced macro renames: function definitions, function prototypes
 * and object declarations provided from an eligible header get
 * emitted in rewritten form if they invoke a renamed macro.
 */
#include "force-macro-rename-3.h"

void pu_f(void)
{
	int i = h1() + h2();
	o1 = i;
	o2 = i;
	ee_o3 = i;
	ee_f1();
	ee_f2();
	o4 = get_o4();
}
/* cp-out: warning: [^:]*: line 14, column 11: referenced object "o1" cannot get externalized */
/* cp-out: warning: [^:]*: line 15, column 11: referenced object "o2" cannot get externalized */
/* cp-out: warning: [^:]*: line 31, columns 0-13: referenced object "o4" cannot get externalized */
