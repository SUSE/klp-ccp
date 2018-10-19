/* PR c/67730 */
/* { dg-do compile } */
/* { dg-options "" } */

#include "pr67730.h"

extern void bar (int);

int
fn1 (void)
{
  int a = NULL; /* { dg-warning "initialization of 'int' from 'void \\*' makes integer from pointer" } */
  a = NULL; /* { dg-warning "assignment to 'int' from 'void \\*' makes integer from pointer" } */
  bar (NULL); /* { dg-warning "passing argument 1" } */
  return NULL; /* { dg-warning "returning 'void \\*' from a function with return type 'int' makes integer from pointer" } */
}

int
fn2 (void)
{
  RETURN; /* { dg-warning "returning 'void \\*' from a function with return type 'int' makes integer from pointer" } */
}

/* cp-out: warning: .*.h: line 22, columns 10-14: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 23, columns 6-10: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 24, columns 11-15: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 25, columns 9-13: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 31, columns 2-8: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 12, columns 10-14: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 13, columns 6-10: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 14, columns 7-11: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 15, columns 9-13: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 21, columns 2-8: assignment to integer from pointer */
