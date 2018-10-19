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

/* cp-out: warning: .*.h: line 22, columns 10-14: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 23, columns 6-10: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 24, columns 11-15: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 25, columns 9-13: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 31, columns 2-8: assignment to integer from pointer */
