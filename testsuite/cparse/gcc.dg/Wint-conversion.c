/* { dg-do compile } */
/* { dg-options "-Wno-int-conversion" } */

int fn1 (int *), *fn2 (int);

int
fn1 (int *p)
{
  int i = p;
  i = p;
  fn2 (p);
  return p;
}

int *
fn2 (int i)
{
  int *p = i;
  p = i;
  fn1 (i);
  return i;
}

/* cp-out: warning: [^:]*: line 9, columns 10-11: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 10, columns 6-7: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 11, columns 7-8: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 12, columns 9-10: assignment to integer from pointer */
/* cp-out: warning: [^:]*: line 18, columns 11-12: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 19, columns 6-7: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 20, columns 7-8: assignment to pointer from integer which is not NULL */
/* cp-out: warning: [^:]*: line 21, columns 9-10: assignment to pointer from integer which is not NULL */
