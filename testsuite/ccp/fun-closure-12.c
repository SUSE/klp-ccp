#include "fun-closure-12.h"

void pu_f(void)
{
	/*
	 * ee_g() remains in the closure and warnings about its
	 * non-externalizable dependencies are expected.
	 */
	ee_g();
}

/* cp-out: warning: [^:]*: line 6, column 4: referenced object "a" cannot get externalized */
/* cp-out: warning: [^:]*: line 15, column 21: required function externalization of "k" is impossible */
/* cp-out: warning: [^:]*: line 18, column 1: function "h" without definition cannot get externalized */
