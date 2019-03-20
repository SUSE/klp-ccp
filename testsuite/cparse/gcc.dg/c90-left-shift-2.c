/* { dg-do compile } */
/* { dg-options "-std=iso9899:1990 -pedantic-errors" } */

#define INTM2 (sizeof (int) * __CHAR_BIT__ - 2)

enum { A = 10 << INTM2 };
int k = 10 << INTM2;

void
fn (int i)
{
  switch (i)
  case 10 << INTM2: break;
}

/* cp-out: warning: [^:]*: line 6, columns 11-22: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 7, columns 8-19: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 13, columns 7-18: integer overflow in shift operation */
