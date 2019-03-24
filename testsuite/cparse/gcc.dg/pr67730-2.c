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

/* cp-out: warning: .*.h: line 22, column 10: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 23, column 6: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 24, column 11: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 25, column 9: assignment to integer from pointer */
/* cp-out: warning: .*.h: line 31, column 2: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 12, column 10: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 13, column 6: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 14, column 7: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 15, column 9: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 21, column 2: assignment to integer from pointer */
