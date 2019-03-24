/* PR c/67730 */
/* { dg-do compile } */
/* { dg-options "-Wc++-compat" } */

#include "pr67730.h"

extern void bar (unsigned char *);

unsigned char *
f (void *p)
{
   unsigned char *uc = ONEP; /* { dg-warning "request for implicit conversion" } */
   uc = ONEP; /* { dg-warning "request for implicit conversion" } */
   bar (ONEP); /* { dg-warning "request for implicit conversion" } */
   return ONEP; /* { dg-warning "request for implicit conversion" } */
}

/* cp-out: warning: .*.h: line 22, column 10: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 23, column 6: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 24, column 11: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 25, column 9: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 31, column 2: assignment to integer from pointer */
