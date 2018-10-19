/* { dg-do compile } */
/* { dg-options "-std=iso9899:1990 -pedantic-errors" } */

#define INTM1 (sizeof (int) * __CHAR_BIT__ - 1)

enum { A = 1 << INTM1 };
int k = 1 << INTM1;

void
fn (int i)
{
  switch (i)
  case 1 << INTM1: break;
}

/* cp-out: warning: [^:]*: line 6, columns 11-12: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 7, columns 8-9: integer overflow in shift operation */
/* cp-out: warning: [^:]*: line 13, columns 7-8: integer overflow in shift operation */
